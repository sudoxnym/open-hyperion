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

    void on_LineEdit_updated(QObject* lineedit_argument);

    void on_CheckBox_updated(QObject* checkbox_argument);

    void on_ButtonAddController_clicked();

    void on_ButtonRemoveController_clicked();

    void on_ButtonAddUniverse_clicked();

    void on_ButtonRemoveUniverse_clicked();

    void on_ButtonAutoMap_clicked();

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
