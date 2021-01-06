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

private:
    ResourceManager* resource_manager;
    Ui::OpenRGBE131ReceiverDialog *ui;

    std::thread*     E131ReceiverThread;
};

#endif // OPENRGBE131RECEIVERDIALOG_H
