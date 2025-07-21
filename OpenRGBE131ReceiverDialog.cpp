#include "OpenRGBE131ReceiverDialog.h"
#include "ui_OpenRGBE131ReceiverDialog.h"

#include <QDir>
#include <QLineEdit>
#include <QMessageBox>
#include <QSignalMapper>
#include "filesystem.h"
#include "RGBController.h"
#include <e131.h>
#include <fstream>
#include <nlohmann/json.hpp>

#ifndef _WIN32
#include <unistd.h>
#define closesocket ::close
#endif

#define MAX_LEDS_PER_UNIVERSE  170

using json = nlohmann::json;

void DeviceListChanged_Callback(void * this_ptr)
{
    OpenRGBE131ReceiverDialog * this_obj = (OpenRGBE131ReceiverDialog *)this_ptr;

    QMetaObject::invokeMethod(this_obj, "DeviceListChanged", Qt::QueuedConnection);
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

typedef unsigned int LineEditParameter;
enum
{
    LINEEDIT_PARAMETER_START_CHANNEL,
    LINEEDIT_PARAMETER_START_LED,
    LINEEDIT_PARAMETER_NUM_LEDS
};

class LineEditArgument : public QObject
{
public:
    QWidget *           widget;
    unsigned int        universe_idx;
    unsigned int        member_idx;
    LineEditParameter   parameter;
};

typedef unsigned int CheckBoxParameter;
enum
{
    CHECKBOX_PARAMETER_UPDATE
};

class CheckBoxArgument : public QObject
{
public:
    QWidget *           widget;
    unsigned int        universe_idx;
    unsigned int        member_idx;
    CheckBoxParameter   parameter;
};

OpenRGBE131ReceiverDialog::OpenRGBE131ReceiverDialog(ResourceManagerInterface* manager, QWidget *parent) : QWidget(parent),  ui(new Ui::OpenRGBE131ReceiverDialog)
{
    ui->setupUi(this);

    resource_manager = manager;

    /*-------------------------------------------------*\
    | Register device list change callback              |
    \*-------------------------------------------------*/
    resource_manager->RegisterDeviceListChangeCallback(DeviceListChanged_Callback, this);
    resource_manager->RegisterDetectionProgressCallback(DeviceListChanged_Callback, this);

    online         = false;
    initialized    = false;
    received_count = 0;

    UpdateOnlineStatus();
}

OpenRGBE131ReceiverDialog::~OpenRGBE131ReceiverDialog()
{
    /*-------------------------------------------------*\
    | Register device list change callback              |
    \*-------------------------------------------------*/
    resource_manager->UnregisterDeviceListChangeCallback(DeviceListChanged_Callback, this);
    resource_manager->UnregisterDetectionProgressCallback(DeviceListChanged_Callback, this);
    
    delete ui;
}

void OpenRGBE131ReceiverDialog::DeviceListChanged()
{
    if(resource_manager->GetDetectionPercent() == 100)
    {
        ui->ControllersTreeView->clear();
        ui->E131TreeView->clear();
        universe_list.clear();

        UpdateControllersTreeView();
        LoadMap();

        if (!initialized)
        {
            if (ui->EnableAutoStartBox->checkState() == Qt::Checked)
            {
                this->StartReceiver();
            }
            initialized = true;
        }
    }
    else
    {
        ui->E131TreeView->clear();
        universe_list.clear();
    }
}

void OpenRGBE131ReceiverDialog::UpdateControllersTreeView()
{
    ui->ControllersTreeView->clear();

    ui->ControllersTreeView->setColumnCount(1);
    ui->ControllersTreeView->setHeaderLabels(QStringList() << "Controller");

    for(unsigned int controller_idx = 0; controller_idx < resource_manager->GetRGBControllers().size(); controller_idx++)
    {
        RGBController* controller   = resource_manager->GetRGBControllers()[controller_idx];

        QTreeWidgetItem* new_controller_entry = new QTreeWidgetItem(ui->ControllersTreeView);

        new_controller_entry->setText(0, QString::fromStdString(controller->name));
    }
}

void OpenRGBE131ReceiverDialog::UpdateTreeView()
{
    ui->E131TreeView->clear();

    ui->E131TreeView->setColumnCount(5);
    ui->E131TreeView->header()->setStretchLastSection(false);
    ui->E131TreeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->E131TreeView->setHeaderLabels(QStringList() << "Universe" << "Start Channel" << "Start LED" << "LED Count" << "Update");

    /*-----------------------------------------------------*\
    | Create a signal mapper for LineEdit fields            |
    \*-----------------------------------------------------*/
    QSignalMapper* LineEditMapper = new QSignalMapper(this);
    connect(LineEditMapper, SIGNAL(mapped(QObject *)), this, SLOT(LineEdit_updated_slot(QObject *)));

    /*-----------------------------------------------------*\
    | Create a signal mapper for CheckBox fields            |
    \*-----------------------------------------------------*/
    QSignalMapper* CheckBoxMapper = new QSignalMapper(this);
    connect(CheckBoxMapper, SIGNAL(mapped(QObject *)), this, SLOT(CheckBox_updated_slot(QObject *)));

    for(unsigned int universe_idx = 0; universe_idx < universe_list.size(); universe_idx++)
    {
        QTreeWidgetItem* new_universe_entry = new QTreeWidgetItem(ui->E131TreeView);

        new_universe_entry->setText(0, QString::fromStdString("Universe " + std::to_string(universe_list[universe_idx].universe)));

        for(unsigned int member_idx = 0; member_idx < universe_list[universe_idx].members.size(); member_idx++)
        {
            QTreeWidgetItem* new_member_entry = new QTreeWidgetItem(new_universe_entry);

            new_member_entry->setText(0, QString::fromStdString((universe_list[universe_idx].members[member_idx].controller->name)));

            QLineEdit* start_channel_edit = new QLineEdit(ui->E131TreeView);
            start_channel_edit->setText(QString::number(universe_list[universe_idx].members[member_idx].start_channel));
            ui->E131TreeView->setItemWidget(new_member_entry, 1, start_channel_edit);

            LineEditArgument* start_channel_argument = new LineEditArgument();

            start_channel_argument->widget          = start_channel_edit;
            start_channel_argument->universe_idx    = universe_idx;
            start_channel_argument->member_idx      = member_idx;
            start_channel_argument->parameter       = LINEEDIT_PARAMETER_START_CHANNEL;

            connect(start_channel_edit, SIGNAL(editingFinished()), LineEditMapper, SLOT(map()));
            LineEditMapper->setMapping(start_channel_edit, start_channel_argument);

            QLineEdit* start_led_edit = new QLineEdit(ui->E131TreeView);
            start_led_edit->setText(QString::number(universe_list[universe_idx].members[member_idx].start_led));
            ui->E131TreeView->setItemWidget(new_member_entry, 2, start_led_edit);

            LineEditArgument* start_led_argument = new LineEditArgument();

            start_led_argument->widget          = start_led_edit;
            start_led_argument->universe_idx    = universe_idx;
            start_led_argument->member_idx      = member_idx;
            start_led_argument->parameter       = LINEEDIT_PARAMETER_START_LED;

            connect(start_led_edit, SIGNAL(editingFinished()), LineEditMapper, SLOT(map()));
            LineEditMapper->setMapping(start_led_edit, start_led_argument);

            QLineEdit* num_leds_edit = new QLineEdit(ui->E131TreeView);
            num_leds_edit->setText(QString::number(universe_list[universe_idx].members[member_idx].num_leds));
            ui->E131TreeView->setItemWidget(new_member_entry, 3, num_leds_edit);

            LineEditArgument* num_leds_argument = new LineEditArgument();

            num_leds_argument->widget          = num_leds_edit;
            num_leds_argument->universe_idx    = universe_idx;
            num_leds_argument->member_idx      = member_idx;
            num_leds_argument->parameter       = LINEEDIT_PARAMETER_NUM_LEDS;

            connect(num_leds_edit, SIGNAL(editingFinished()), LineEditMapper, SLOT(map()));
            LineEditMapper->setMapping(num_leds_edit, num_leds_argument);

            QCheckBox* update_checkbox = new QCheckBox(ui->E131TreeView);
            update_checkbox->setChecked(universe_list[universe_idx].members[member_idx].update);
            ui->E131TreeView->setItemWidget(new_member_entry, 4, update_checkbox);

            CheckBoxArgument* update_argument = new CheckBoxArgument();

            update_argument->widget          = update_checkbox;
            update_argument->universe_idx    = universe_idx;
            update_argument->member_idx      = member_idx;
            update_argument->parameter       = CHECKBOX_PARAMETER_UPDATE;

            connect(update_checkbox, SIGNAL(clicked()), CheckBoxMapper, SLOT(map()));
            CheckBoxMapper->setMapping(update_checkbox, update_argument);
        }
    }

    ui->E131TreeView->setStyleSheet("QLineEdit { border: none }");

    ui->E131TreeView->expandAll();
}

void OpenRGBE131ReceiverDialog::AutoMap()
{
    for(unsigned int controller_idx = 0; controller_idx < resource_manager->GetRGBControllers().size(); controller_idx++)
    {
        RGBController* controller   = resource_manager->GetRGBControllers()[controller_idx];

        // Determine if the controller has a Direct mode
        bool has_direct = false;
        for(unsigned int mode_idx = 0; mode_idx < controller->modes.size(); mode_idx++)
        {
            if(controller->modes[mode_idx].name == "Direct")
            {
                has_direct = true;
                break;
            }
        }

        // Only map controllers that have a Direct mode
        if(has_direct)
        {
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
                new_member.update        = false;

                // Limit the number of LEDs
                if(new_member.num_leds > MAX_LEDS_PER_UNIVERSE)
                {
                    new_member.num_leds  = MAX_LEDS_PER_UNIVERSE;
                }

                // Update start LED
                start_led                = start_led + new_member.num_leds;

                // Update remaining LED count
                remaining_leds           = remaining_leds - new_member.num_leds;

                // Only set the update flag for the last universe of the controller
                if(universe_idx == (num_universes - 1))
                {
                    new_member.update   = true;
                }

                new_entry.members.push_back(new_member);

                universe_list.push_back(new_entry);
            }
        }
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
    | Join the configured universes if multicast enabled    |
    \*-----------------------------------------------------*/
    bool multicast = ui->EnableMulticastBox->checkState() == Qt::Checked;

    for(unsigned int universe_idx = 0; universe_idx < universe_list.size(); universe_idx++)
    {
        if(multicast)
        {
            if(e131_multicast_join(sockfd, universe_list[universe_idx].universe) < 0)
            {
                printf("Join error\r\n");
                return;
            }
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
            ui->ReceiverSourceValue->setText(QString::fromLocal8Bit((const char *)packet.frame.source_name, 64));

            last_update_time = std::chrono::steady_clock::now();
        }

        last_seq = packet.frame.seq_number;

        /*-------------------------------------------------*\
        | If received packet is from a valid universe,      |
        | update the corresponding devices                  |
        \*-------------------------------------------------*/
        unsigned int universe = ntohs(packet.frame.universe);

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

                    // Channel range is 1-512, so break if channel is zero
                    if(channel == 0)
                    {
                        break;
                    }

                    for(unsigned int led_idx = start_led; led_idx < (start_led + num_leds); led_idx++)
                    {
                        // Calculate channels for this LED
                        unsigned int red_idx    = channel + 0;
                        unsigned int grn_idx    = channel + 1;
                        unsigned int blu_idx    = channel + 2;

                        // Verify last channel (blue) is within range 1-512
                        if(blu_idx > 512)
                        {
                            break;
                        }

                        // Get color out of E1.31 packet
                        RGBColor led_color      = ToRGBColor(packet.dmp.prop_val[red_idx],
                                                             packet.dmp.prop_val[grn_idx],
                                                             packet.dmp.prop_val[blu_idx]);

                        // Verify LED index
                        if(led_idx >= controller->colors.size())
                        {
                            break;
                        }

                        // Set LED color in controller
                        controller->colors[led_idx] = led_color;

                        // Increment the channel by 3 to account for red, green, and blue channels
                        channel = channel + 3;
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

void OpenRGBE131ReceiverDialog::LoadMap()
{
    /*-----------------------------------------------------*\
    | Read in the JSON map from a file                      |
    \*-----------------------------------------------------*/
    json universe_map;

    /*---------------------------------------------------------*\
    | Open input file in binary mode                            |
    \*---------------------------------------------------------*/
    std::ifstream universe_file(resource_manager->GetConfigurationDirectory() / "plugins"
                                    / "settings" / "E131UniverseMap.json",
                                std::ios::in | std::ios::binary);

    /*---------------------------------------------------------*\
    | Read settings into JSON store                             |
    \*---------------------------------------------------------*/
    if (universe_file) {
        try {
            universe_file >> universe_map;
        } catch (std::exception e) {
            /*-------------------------------------------------*\
            | If an exception was caught, that means the JSON   |
            | parsing failed.  Clear out any data in the store  |
            | as it is corrupt.                                 |
            \*-------------------------------------------------*/
            universe_map.clear();
        }
    }

    /*---------------------------------------------------------*\
    | Load checkboxes state                                     |
    \*---------------------------------------------------------*/
    if (universe_map.contains("enable_multicast")) {
        if (universe_map["enable_multicast"]) {
            ui->EnableMulticastBox->setCheckState(Qt::Checked);
        } else {
            ui->EnableMulticastBox->setCheckState(Qt::Unchecked);
        }
    }
    if (universe_map.contains("enable_autostart")) {
        if (universe_map["enable_autostart"]) {
            ui->EnableAutoStartBox->setCheckState(Qt::Checked);
        } else {
            ui->EnableAutoStartBox->setCheckState(Qt::Unchecked);
        }
    }

    /*---------------------------------------------------------*\
    | Clear the universe map                                    |
    \*---------------------------------------------------------*/
    universe_list.clear();

    /*---------------------------------------------------------*\
    | Loop through all saved universes and create list entries  |
    \*---------------------------------------------------------*/
    if (universe_map.contains("universes")) {
        for (unsigned int universe_index = 0; universe_index < universe_map["universes"].size();
             universe_index++) {
            if (universe_map["universes"][universe_index].contains("universe")) {
                universe_entry new_universe;

                new_universe.universe = universe_map["universes"][universe_index]["universe"];

                if (universe_map["universes"][universe_index].contains("members")) {
                    for (unsigned int member_index = 0;
                         member_index < universe_map["universes"][universe_index]["members"].size();
                         member_index++) {
                        universe_member new_member;

                        /*---------------------------------------------------------*\
                        | Fill in default values in case entries are missing        |
                        \*---------------------------------------------------------*/
                        new_member.controller = NULL;
                        new_member.start_channel = 1;
                        new_member.start_led = 0;
                        new_member.num_leds = 0;
                        new_member.update = false;

                        if (universe_map["universes"][universe_index]["members"][member_index]
                                .contains("start_channel")) {
                            new_member.start_channel
                                = universe_map["universes"][universe_index]["members"][member_index]
                                              ["start_channel"];
                        }

                        if (universe_map["universes"][universe_index]["members"][member_index]
                                .contains("start_led")) {
                            new_member.start_led
                                = universe_map["universes"][universe_index]["members"][member_index]
                                              ["start_led"];
                        }

                        if (universe_map["universes"][universe_index]["members"][member_index]
                                .contains("num_leds")) {
                            new_member.num_leds = universe_map["universes"][universe_index]
                                                              ["members"][member_index]["num_leds"];
                        }

                        if (universe_map["universes"][universe_index]["members"][member_index]
                                .contains("update")) {
                            new_member.update = universe_map["universes"][universe_index]["members"]
                                                            [member_index]["update"];
                        }

                        /*---------------------------------------------------------*\
                        | Get controller information                                |
                        \*---------------------------------------------------------*/
                        std::string controller_name = "";
                        std::string controller_description = "";
                        std::string controller_location = "";
                        std::string controller_serial = "";
                        unsigned int controller_led_count = 0;

                        if (universe_map["universes"][universe_index]["members"][member_index]
                                .contains("controller_name")) {
                            controller_name = universe_map["universes"][universe_index]["members"]
                                                          [member_index]["controller_name"];
                        }

                        if (universe_map["universes"][universe_index]["members"][member_index]
                                .contains("controller_description")) {
                            controller_description
                                = universe_map["universes"][universe_index]["members"][member_index]
                                              ["controller_description"];
                        }

                        if (universe_map["universes"][universe_index]["members"][member_index]
                                .contains("controller_location")) {
                            controller_location
                                = universe_map["universes"][universe_index]["members"][member_index]
                                              ["controller_location"];
                        }

                        if (universe_map["universes"][universe_index]["members"][member_index]
                                .contains("controller_serial")) {
                            controller_serial = universe_map["universes"][universe_index]["members"]
                                                            [member_index]["controller_serial"];
                        }

                        if (universe_map["universes"][universe_index]["members"][member_index]
                                .contains("controller_led_count")) {
                            controller_led_count
                                = universe_map["universes"][universe_index]["members"][member_index]
                                              ["controller_led_count"];
                        }

                        /*---------------------------------------------------------*\
                        | Search the controller list for a matching controller      |
                        \*---------------------------------------------------------*/
                        for (unsigned int controller_index = 0;
                             controller_index < resource_manager->GetRGBControllers().size();
                             controller_index++) {
                            if ((resource_manager->GetRGBControllers()[controller_index]->name
                                 == controller_name)
                                && (resource_manager->GetRGBControllers()[controller_index]->description
                                    == controller_description)
                                && (resource_manager->GetRGBControllers()[controller_index]->location
                                    == controller_location)
                                && (resource_manager->GetRGBControllers()[controller_index]->serial
                                    == controller_serial)
                                && (resource_manager->GetRGBControllers()[controller_index]
                                        ->colors.size()
                                    == controller_led_count)) {
                                new_member.controller = resource_manager
                                                            ->GetRGBControllers()[controller_index];
                            }
                        }

                        /*---------------------------------------------------------*\
                        | If a controller was found, add the member to the universe |
                        \*---------------------------------------------------------*/
                        if (new_member.controller != NULL) {
                            new_universe.members.push_back(new_member);
                        }
                    }
                }

                universe_list.push_back(new_universe);
            }
        }
    }

    /*-----------------------------------------------------*\
    | Update the universe tree view                         |
    \*-----------------------------------------------------*/
    UpdateTreeView();
}

void OpenRGBE131ReceiverDialog::StartReceiver()
{
    if (!online) {
        // Start the receiver thread
        E131ReceiverThread = new std::thread(&OpenRGBE131ReceiverDialog::E131ReceiverThreadFunction, this);
    }
}

void OpenRGBE131ReceiverDialog::on_ButtonStartReceiver_clicked()
{
    StartReceiver();
}

void OpenRGBE131ReceiverDialog::on_ButtonStopReceiver_clicked()
{
    online = false;

    UpdateOnlineStatus();
}

void OpenRGBE131ReceiverDialog::LineEdit_updated_slot(QObject* lineedit_argument)
{
    /*-----------------------------------------------------*\
    | Update the parameter                                  |
    \*-----------------------------------------------------*/
    unsigned int universe_idx   = ((LineEditArgument*)lineedit_argument)->universe_idx;
    unsigned int member_idx     = ((LineEditArgument*)lineedit_argument)->member_idx;
    LineEditParameter parameter = ((LineEditArgument*)lineedit_argument)->parameter;
    QWidget* widget             = ((LineEditArgument*)lineedit_argument)->widget;

    unsigned int value          = ((QLineEdit*)widget)->text().toInt();

    switch(parameter)
    {
        case LINEEDIT_PARAMETER_START_CHANNEL:
            universe_list[universe_idx].members[member_idx].start_channel = value;
            break;

        case LINEEDIT_PARAMETER_START_LED:
            universe_list[universe_idx].members[member_idx].start_led = value;
            break;

        case LINEEDIT_PARAMETER_NUM_LEDS:
            universe_list[universe_idx].members[member_idx].num_leds = value;
            break;
    }
}

void OpenRGBE131ReceiverDialog::CheckBox_updated_slot(QObject* checkbox_argument)
{
    /*-----------------------------------------------------*\
    | Update the parameter                                  |
    \*-----------------------------------------------------*/
    unsigned int universe_idx   = ((CheckBoxArgument*)checkbox_argument)->universe_idx;
    unsigned int member_idx     = ((CheckBoxArgument*)checkbox_argument)->member_idx;
    CheckBoxParameter parameter = ((CheckBoxArgument*)checkbox_argument)->parameter;
    QWidget* widget             = ((CheckBoxArgument*)checkbox_argument)->widget;

    bool value                  = ((QCheckBox*)widget)->isChecked();

    switch(parameter)
    {
        case CHECKBOX_PARAMETER_UPDATE:
            universe_list[universe_idx].members[member_idx].update = value;
            break;
    }
}

void OpenRGBE131ReceiverDialog::on_ButtonAddController_clicked()
{
    /*-----------------------------------------------------*\
    | Make sure a valid controller is selected on the left  |
    | and a valid universe is selected on the right         |
    \*-----------------------------------------------------*/
    int selected_controller = ui->ControllersTreeView->indexOfTopLevelItem(ui->ControllersTreeView->currentItem());
    int selected_universe   = ui->E131TreeView->indexOfTopLevelItem(ui->E131TreeView->currentItem());

    if((selected_controller == -1) || (selected_universe == -1))
    {
        return;
    }

    /*-----------------------------------------------------*\
    | Determine start channel                               |
    \*-----------------------------------------------------*/
    unsigned int start_channel  = 1;

    if(universe_list[selected_universe].members.size() > 0)
    {
        universe_member last_member = universe_list[selected_universe].members[universe_list[selected_universe].members.size() - 1];
        start_channel               = last_member.start_channel + (last_member.num_leds * 3);
    }

    /*-----------------------------------------------------*\
    | Determine number of LEDs                              |
    \*-----------------------------------------------------*/
    unsigned int start_led      = 0;
    unsigned int num_leds       = resource_manager->GetRGBControllers()[selected_controller]->colors.size();
    unsigned int remaining_leds = num_leds;
    bool         update         = false;

    if((num_leds * 3) + start_channel > 512)
    {
        num_leds       = (512 - start_channel) / 3;
        remaining_leds -= num_leds;
        start_led      += num_leds;

        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(this, "", "This controller is too large for the selected universe.  Do you want to map additional universes for this controller?", QMessageBox::Yes|QMessageBox::No);

        if(reply == QMessageBox::Yes)
        {
            update  = false;
        }
        else
        {
            update  = true;
        }
    }
    else
    {
        update = true;
    }

    /*-----------------------------------------------------*\
    | Add controller to universe                            |
    \*-----------------------------------------------------*/
    universe_member new_member;

    new_member.controller                   = resource_manager->GetRGBControllers()[selected_controller];
    new_member.start_channel                = start_channel;
    new_member.start_led                    = 0;
    new_member.num_leds                     = num_leds;

    new_member.update                       = update;

    universe_list[selected_universe].members.push_back(new_member);

    /*-----------------------------------------------------*\
    | Add additional universes for controller               |
    \*-----------------------------------------------------*/
    if(update == false)
    {
        while(remaining_leds > 0)
        {
            start_channel   = 1;
            num_leds        = remaining_leds;

            if((num_leds * 3) + start_channel > 512)
            {
                num_leds       = (512 - start_channel) / 3;
            }

            remaining_leds -= num_leds;

            if(remaining_leds == 0)
            {
                update          = true;
            }

            /*-----------------------------------------------------*\
            | Get next universe value                               |
            \*-----------------------------------------------------*/
            unsigned int next_universe = 1;

            if(universe_list.size() > 0)
            {
                next_universe = universe_list[universe_list.size() - 1].universe + 1;
            }

            /*-----------------------------------------------------*\
            | Create and add new universe to list                   |
            \*-----------------------------------------------------*/
            universe_entry new_universe;

            new_universe.universe = next_universe;

            /*-----------------------------------------------------*\
            | Add selected controller member to new universe        |
            \*-----------------------------------------------------*/
            universe_member new_member;

            new_member.controller                   = resource_manager->GetRGBControllers()[selected_controller];
            new_member.start_channel                = 1;
            new_member.start_led                    = start_led;
            new_member.num_leds                     = num_leds;
            new_member.update                       = update;

            new_universe.members.push_back(new_member);

            universe_list.push_back(new_universe);

            start_led += num_leds;
        }
    }

    /*-----------------------------------------------------*\
    | Update the universe tree view                         |
    \*-----------------------------------------------------*/
    UpdateTreeView();
}

void OpenRGBE131ReceiverDialog::on_ButtonRemoveController_clicked()
{
    /*-----------------------------------------------------*\
    | Make selected item is not a top-level (universe)      |
    \*-----------------------------------------------------*/
    int selected_controller = ui->E131TreeView->indexOfTopLevelItem(ui->E131TreeView->currentItem());

    if(selected_controller != -1)
    {
        return;
    }

    /*-----------------------------------------------------*\
    | Get index of universe member                          |
    \*-----------------------------------------------------*/
    selected_controller     = ui->E131TreeView->currentIndex().row();
    int selected_universe   = ui->E131TreeView->indexOfTopLevelItem(ui->E131TreeView->currentItem()->parent());

    if(selected_universe == -1)
    {
        return;
    }

    /*-----------------------------------------------------*\
    | Remove controller member from universe                |
    \*-----------------------------------------------------*/
    universe_list[selected_universe].members.erase(universe_list[selected_universe].members.begin() + selected_controller);

    /*-----------------------------------------------------*\
    | Update the universe tree view                         |
    \*-----------------------------------------------------*/
    UpdateTreeView();
}

void OpenRGBE131ReceiverDialog::on_ButtonAddUniverse_clicked()
{
    /*-----------------------------------------------------*\
    | Get next universe value                               |
    \*-----------------------------------------------------*/
    unsigned int next_universe = 1;

    if(universe_list.size() > 0)
    {
        next_universe = universe_list[universe_list.size() - 1].universe + 1;
    }

    /*-----------------------------------------------------*\
    | Create and add new universe to list                   |
    \*-----------------------------------------------------*/
    universe_entry new_universe;

    new_universe.universe = next_universe;

    universe_list.push_back(new_universe);

    /*-----------------------------------------------------*\
    | Update the universe tree view                         |
    \*-----------------------------------------------------*/
    UpdateTreeView();
}

void OpenRGBE131ReceiverDialog::on_ButtonRemoveUniverse_clicked()
{
    /*-----------------------------------------------------*\
    | Get index of universe                                 |
    \*-----------------------------------------------------*/
    int selected_universe   = ui->E131TreeView->indexOfTopLevelItem(ui->E131TreeView->currentItem());

    if(selected_universe == -1)
    {
        return;
    }

    /*-----------------------------------------------------*\
    | Remove universe                                       |
    \*-----------------------------------------------------*/
    universe_list.erase(universe_list.begin() + selected_universe);

    /*-----------------------------------------------------*\
    | Update the universe tree view                         |
    \*-----------------------------------------------------*/
    UpdateTreeView();
}

void OpenRGBE131ReceiverDialog::on_ButtonAutoMap_clicked()
{
    AutoMap();

    /*-----------------------------------------------------*\
    | Update the universe tree view                         |
    \*-----------------------------------------------------*/
    UpdateTreeView();
}

void OpenRGBE131ReceiverDialog::on_ButtonSaveMap_clicked()
{
    /*-----------------------------------------------------*\
    | Create a JSON structure to hold the universe map      |
    \*-----------------------------------------------------*/
    json universe_map;

    /*-----------------------------------------------------*\
    | Loop through the universe list and add universe fields|
    \*-----------------------------------------------------*/
    for(unsigned int universe_index = 0; universe_index < universe_list.size(); universe_index++)
    {
        universe_entry universe = universe_list[universe_index];

        universe_map["universes"][universe_index]["universe"] = universe.universe;

        /*-----------------------------------------------------*\
        | Loop through all members and add info fields          |
        \*-----------------------------------------------------*/
        for(unsigned int member_index = 0; member_index < universe.members.size(); member_index++)
        {
            universe_member member = universe.members[member_index];

            universe_map["universes"][universe_index]["members"][member_index]["start_channel"]             = member.start_channel;
            universe_map["universes"][universe_index]["members"][member_index]["start_led"]                 = member.start_led;
            universe_map["universes"][universe_index]["members"][member_index]["num_leds"]                  = member.num_leds;
            universe_map["universes"][universe_index]["members"][member_index]["update"]                    = member.update;
            universe_map["universes"][universe_index]["members"][member_index]["controller_name"]           = member.controller->name;
            universe_map["universes"][universe_index]["members"][member_index]["controller_description"]    = member.controller->description;
            universe_map["universes"][universe_index]["members"][member_index]["controller_location"]       = member.controller->location;
            universe_map["universes"][universe_index]["members"][member_index]["controller_serial"]         = member.controller->serial;
            universe_map["universes"][universe_index]["members"][member_index]["controller_led_count"]      = member.controller->colors.size();
        }
    }

    universe_map["enable_multicast"] = ui->EnableMulticastBox->checkState() == Qt::Checked;
    universe_map["enable_autostart"] = ui->EnableAutoStartBox->checkState() == Qt::Checked;

    /*-----------------------------------------------------*\
    | Write out the JSON structure to a file                |
    \*-----------------------------------------------------*/
    filesystem::path settings_dir = resource_manager->GetConfigurationDirectory() / "plugins" / "settings";

    if (!std::filesystem::is_directory(settings_dir) || !std::filesystem::exists(settings_dir)) {
        std::filesystem::create_directories(settings_dir);
    }

    std::ofstream universe_file(settings_dir / "E131UniverseMap.json", std::ios::out | std::ios::binary);

    if(universe_file)
    {
        try
        {
            universe_file << universe_map.dump(4);
        }
        catch(std::exception e)
        {

        }

        universe_file.close();
    }
}

void OpenRGBE131ReceiverDialog::on_ButtonLoadMap_clicked()
{
    this->LoadMap();
}
