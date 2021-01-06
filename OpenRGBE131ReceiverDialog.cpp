#include "OpenRGBE131ReceiverDialog.h"
#include "ui_OpenRGBE131ReceiverDialog.h"

#include <e131.h>

#define MAX_LEDS_PER_UNIVERSE  170

void DeviceListChanged_Callback(void * this_ptr)
{
    ((OpenRGBE131ReceiverDialog *)this_ptr)->DeviceListChanged();
}

typedef struct
{
    RGBController*                  controller;
    unsigned int                    start_channel;
    unsigned int                    start_led;
    unsigned int                    num_leds;
    bool                            update;
} universe_member;

typedef struct
{
    unsigned int                    universe;
    std::vector<universe_member>    members;
} universe_entry;

static std::vector<universe_entry> universe_list;

OpenRGBE131ReceiverDialog::OpenRGBE131ReceiverDialog(ResourceManager* manager, QWidget *parent) : QWidget(parent),  ui(new Ui::OpenRGBE131ReceiverDialog)
{
    ui->setupUi(this);

    resource_manager = manager;

    /*-------------------------------------------------*\
    | Register device list change callback              |
    \*-------------------------------------------------*/
    resource_manager->RegisterDeviceListChangeCallback(DeviceListChanged_Callback, this);
    resource_manager->RegisterDetectionProgressCallback(DeviceListChanged_Callback, this);

    online         = false;
    received_count = 0;

    UpdateOnlineStatus();
}

OpenRGBE131ReceiverDialog::~OpenRGBE131ReceiverDialog()
{
    delete ui;
}

void OpenRGBE131ReceiverDialog::DeviceListChanged()
{
    if(resource_manager->GetDetectionPercent() == 100)
    {
        universe_list.clear();

        for(unsigned int controller_idx = 0; controller_idx < resource_manager->GetRGBControllers().size(); controller_idx++)
        {
            RGBController* controller   = resource_manager->GetRGBControllers()[controller_idx];

            unsigned int num_universes  = 1 + ((controller->leds.size() * 3) / 512);
            unsigned int remaining_leds = controller->leds.size();
            unsigned int start_led      = 0;

            for(unsigned int universe_idx = 0; universe_idx < num_universes; universe_idx++)
            {
                // For now, create universes sequentially per controller
                universe_entry new_entry;

                new_entry.universe = universe_list.size() + 1;

                // Add members as needed
                universe_member new_member;

                new_member.controller    = resource_manager->GetRGBControllers()[controller_idx];
                new_member.start_channel = 1;
                new_member.start_led     = start_led;
                new_member.num_leds      = remaining_leds;
                new_member.update        = 1;

                // Limit the number of LEDs
                if(new_member.num_leds > MAX_LEDS_PER_UNIVERSE)
                {
                    new_member.num_leds  = MAX_LEDS_PER_UNIVERSE;
                }

                // Update start LED
                start_led                = start_led + new_member.num_leds;

                // Update remaining LED count
                remaining_leds           = remaining_leds - new_member.num_leds;

                new_entry.members.push_back(new_member);

                universe_list.push_back(new_entry);
            }
        }

        ui->E131TreeView->clear();

        ui->E131TreeView->setColumnCount(5);
        ui->E131TreeView->header()->setStretchLastSection(false);
        ui->E131TreeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->E131TreeView->setHeaderLabels(QStringList() << "Universe" << "Start Channel" << "Start LED" << "LED Count" << "Update");

        for(unsigned int universe_idx = 0; universe_idx < universe_list.size(); universe_idx++)
        {
            QTreeWidgetItem* new_universe_entry = new QTreeWidgetItem(ui->E131TreeView);

            new_universe_entry->setText(0, QString::fromStdString("Universe " + std::to_string(universe_list[universe_idx].universe)));

            for(unsigned int member_idx = 0; member_idx < universe_list[universe_idx].members.size(); member_idx++)
            {
                QTreeWidgetItem* new_member_entry = new QTreeWidgetItem(new_universe_entry);

                new_member_entry->setText(0, QString::fromStdString((universe_list[universe_idx].members[member_idx].controller->name)));
                new_member_entry->setText(1, QString::number(universe_list[universe_idx].members[member_idx].start_channel));
                new_member_entry->setText(2, QString::number(universe_list[universe_idx].members[member_idx].start_led));
                new_member_entry->setText(3, QString::number(universe_list[universe_idx].members[member_idx].num_leds));
                new_member_entry->setText(4, QString::number(universe_list[universe_idx].members[member_idx].update));
            }
        }

        ui->E131TreeView->expandAll();
    }
}

void OpenRGBE131ReceiverDialog::E131ReceiverThreadFunction()
{
    int             sockfd;
    e131_packet_t   packet;
    e131_error_t    error;
    uint8_t         last_seq = 0x00;
    std::chrono::time_point<std::chrono::steady_clock>  last_update_time;

    /*-----------------------------------------------------*\
    | Initialize update time                                |
    \*-----------------------------------------------------*/
    last_update_time = std::chrono::steady_clock::now();

    /*-----------------------------------------------------*\
    | Clear online status                                   |
    \*-----------------------------------------------------*/
    online = false;

    /*-----------------------------------------------------*\
    | Create a socket for E1.31                             |
    \*-----------------------------------------------------*/
    sockfd = e131_socket();

    if(sockfd < 0)
    {
        printf("Socket error\r\n");
        return;
    }

    /*-----------------------------------------------------*\
    | Bind the socket to the default E1.31 port             |
    \*-----------------------------------------------------*/
    if(e131_bind(sockfd, E131_DEFAULT_PORT) < 0)
    {
        printf("Bind error\r\n");
        return;
    }

    /*-----------------------------------------------------*\
    | Join the configured universes                         |
    \*-----------------------------------------------------*/
    for(unsigned int universe_idx = 0; universe_idx < universe_list.size(); universe_idx++)
    {
        if(e131_multicast_join(sockfd, universe_list[universe_idx].universe) < 0)
        {
            printf("Join error\r\n");
            return;
        }
    }

    /*-----------------------------------------------------*\
    | Set online status                                     |
    \*-----------------------------------------------------*/
    online = true;

    UpdateOnlineStatus();

    /*-----------------------------------------------------*\
    | Loop to receive E1.31 packets                         |
    \*-----------------------------------------------------*/
    while(online)
    {
        if(e131_recv(sockfd, &packet) < 0)
        {
            printf("Receive error\r\n");

            online = false;
        }

        if((error = e131_pkt_validate(&packet)) != E131_ERR_NONE)
        {
            printf("Validation error: %s\r\n", e131_strerror(error));
            continue;
        }

//        if(e131_pkt_discard(&packet, last_seq))
//        {
//            //printf("Warning: packet out of order received\r\n");
//            last_seq = packet.frame.seq_number;
//            continue;
//        }

        received_count++;

        if((std::chrono::steady_clock::now() - last_update_time) > std::chrono::milliseconds(500))
        {
            ui->PacketsReceivedValue->setText(QString::number(received_count));

            last_update_time = std::chrono::steady_clock::now();
        }

        last_seq = packet.frame.seq_number;

        /*-------------------------------------------------*\
        | If received packet is from a valid universe,      |
        | update the corresponding devices                  |
        \*-------------------------------------------------*/
        unsigned int universe = ntohs(packet.frame.universe) - 1;

        for(unsigned int universe_idx = 0; universe_idx < universe_list.size(); universe_idx++)
        {
            if(universe_list[universe_idx].universe == universe)
            {
                for(unsigned int member_idx = 0; member_idx < universe_list[universe_idx].members.size(); member_idx++)
                {
                    RGBController*  controller  = universe_list[universe_idx].members[member_idx].controller;
                    unsigned int    channel     = universe_list[universe_idx].members[member_idx].start_channel;
                    unsigned int    start_led   = universe_list[universe_idx].members[member_idx].start_led;
                    unsigned int    num_leds    = universe_list[universe_idx].members[member_idx].num_leds;
                    bool            update      = universe_list[universe_idx].members[member_idx].update;

                    for(unsigned int led_idx = start_led; led_idx < (start_led + num_leds); led_idx++)
                    {
                        // Calculate channels for this LED
                        unsigned int red_idx    = channel + 0;
                        unsigned int grn_idx    = channel + 1;
                        unsigned int blu_idx    = channel + 2;

                        // Get color out of E1.31 packet
                        RGBColor led_color      = ToRGBColor(packet.dmp.prop_val[red_idx],
                                                             packet.dmp.prop_val[grn_idx],
                                                             packet.dmp.prop_val[blu_idx]);

                        // Set LED color in controller
                        controller->colors[led_idx] = led_color;
                    }

                    // If configured to update the device, update
                    if(update)
                    {
                        controller->UpdateLEDs();
                    }
                }
            }
        }
    }

    closesocket(sockfd);

    UpdateOnlineStatus();
}

void OpenRGBE131ReceiverDialog::UpdateOnlineStatus()
{
    if(online)
    {
        ui->ReceiverStatusValue->setText("Online");

        ui->ButtonStartReceiver->setEnabled(false);
        ui->ButtonStopReceiver->setEnabled(true);
    }
    else
    {
        ui->ReceiverStatusValue->setText("Offline");

        received_count = 0;

        ui->PacketsReceivedValue->setText(QString::number(received_count));

        ui->ButtonStartReceiver->setEnabled(true);
        ui->ButtonStopReceiver->setEnabled(false);
    }
}

void OpenRGBE131ReceiverDialog::on_ButtonStartReceiver_clicked()
{
    if(!online)
    {
        // Start the receiver thread
        E131ReceiverThread = new std::thread(&OpenRGBE131ReceiverDialog::E131ReceiverThreadFunction, this);
    }
}

void OpenRGBE131ReceiverDialog::on_ButtonStopReceiver_clicked()
{
    online = false;

    UpdateOnlineStatus();
}
