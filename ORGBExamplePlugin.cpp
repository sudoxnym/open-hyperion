#include "ORGBExamplePlugin.h"
#include "Dependencies/ResourceManager.h"

PluginInfo ORGBPlugin::DefineNeeded()
{
    ORGBPlugin::PInfo.PluginName = "Example";
    ORGBPlugin::PInfo.PluginDesc = "An example Plugin for OpenRGB";
    ORGBPlugin::PInfo.PluginLoca = "InfoTab";

    ORGBPlugin::PInfo.HasCustom = false;
    ORGBPlugin::PInfo.SettingName = "";

    return ORGBPlugin::PInfo;
}

QLabel* TabLabel()
{
    QLabel *TLabel = new QLabel();
    TLabel->setText("Example");
    return TLabel;
}

PluginInfo ORGBPlugin::init(json Settings, bool DarkTheme)
{
    ORGBPlugin::PInfo.PluginLabel = TabLabel();

    return ORGBPlugin::PInfo;
}

QWidget* ORGBPlugin::CreateGUI(QWidget *Parent)
{
    QWidget *ORGBExamplePage = new QWidget(Parent);
    QLabel *ORGBExampleLabel = new QLabel(ORGBExamplePage);

    QPushButton *ORGBExamplePushButton = new QPushButton(ORGBExamplePage);
    qDebug() << ORGBExamplePushButton->objectName();
    connect(ORGBExamplePushButton,SIGNAL(clicked()) ,this , SLOT(on_ExampleButton_clicked()));

    ORGBExampleLabel->setText("This is an example page added by plugins");
    return ORGBExamplePage;
}

void ORGBPlugin::on_ExampleButton_clicked()
{
    QDialog *ButtonDialog = new QDialog();
    ButtonDialog->setModal(true);
    QLabel *DialogText = new QLabel(ButtonDialog);
    DialogText->setText("This is the result of the button being clicked");
    ButtonDialog->show();
}
