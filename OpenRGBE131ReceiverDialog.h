#ifndef OPENRGBE131RECEIVERDIALOG_H
#define OPENRGBE131RECEIVERDIALOG_H

#include "ResourceManager.h"

#include <QWidget>

namespace Ui {
class OpenRGBE131ReceiverDialog;
}

class OpenRGBE131ReceiverDialog : public QWidget
{
    Q_OBJECT

public:
    explicit OpenRGBE131ReceiverDialog(ResourceManager* manager, QWidget *parent = nullptr);
    ~OpenRGBE131ReceiverDialog();

    void DeviceListChanged();
    void E131ReceiverThreadFunction();

private slots:
    void on_ButtonStartReceiver_clicked();

    void on_ButtonStopReceiver_clicked();

private:
    ResourceManager*                resource_manager;
    Ui::OpenRGBE131ReceiverDialog  *ui;

    bool                            online;
    unsigned long                   received_count;

    std::thread*                    E131ReceiverThread;

    void                            UpdateOnlineStatus();
};

#endif // OPENRGBE131RECEIVERDIALOG_H
