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

    void AutoMap();

    void E131ReceiverThreadFunction();

public slots:
    void DeviceListChanged();

private slots:
    void on_ButtonStartReceiver_clicked();
    void on_ButtonStopReceiver_clicked();
    void on_ButtonAddController_clicked();
    void on_ButtonRemoveController_clicked();
    void on_ButtonAddUniverse_clicked();
    void on_ButtonRemoveUniverse_clicked();
    void on_ButtonAutoMap_clicked();
    void on_ButtonSaveMap_clicked();
    void on_ButtonLoadMap_clicked();

    void LineEdit_updated_slot(QObject* lineedit_argument);
    void CheckBox_updated_slot(QObject* checkbox_argument);

private:
    ResourceManager*                resource_manager;
    Ui::OpenRGBE131ReceiverDialog  *ui;

    bool                            online;
    unsigned long                   received_count;

    std::thread*                    E131ReceiverThread;

    void                            UpdateOnlineStatus();
    void                            UpdateControllersTreeView();
    void                            UpdateTreeView();
};

#endif // OPENRGBE131RECEIVERDIALOG_H
