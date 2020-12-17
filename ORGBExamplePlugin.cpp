#include "ORGBExamplePlugin.h"
#include "Dependencies/ResourceManager.h"

bool ORGBPlugin::HasCustomIcon()
{
    return false;
}

QLabel* ORGBPlugin::TabLabel()
{
    QLabel *TLabel = new QLabel();
    TLabel->setText("Example");
    return TLabel;
}

std::string ORGBPlugin::PluginName()
{
    return "ExamplePlugin";
}

std::string ORGBPlugin::PluginDesc()
{
    return "This is an Example plugin for OpenRGB";
}

std::string ORGBPlugin::PluginLocal()
{
    return "InfoTab";
}

QWidget* ORGBPlugin::CreateGUI(QWidget *Parent, ResourceManager *RM)
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
