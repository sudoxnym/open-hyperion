/*-----------------------------------------*\
|  OpenRGBE131ReceiverPlugin.cpp            |
|                                           |
|  OpenRGB E1.31 Receiver Plugin            |
|                                           |
|  herosilas12 (CoffeeIsLife)   12/11/2020  |
|  Adam Honse (CalcProgrammer1) 1/5/2021    |
\*-----------------------------------------*/

#include "OpenRGBE131ReceiverPlugin.h"
#include "OpenRGBE131ReceiverDialog.h"

/*-----------------------------------------*\
| Initialize                                |
|                                           |
|  This function must be present in all     |
|  OpenRGB plugins.  It defines the plugin  |
|  name, description, location, and other   |
|  plugin information.  It creates the tab  |
|  label and is the entry point for plugin  |
|  code                                     |
\*-----------------------------------------*/
OpenRGBPluginInfo OpenRGBPlugin::Initialize(bool dark_theme, ResourceManager* resource_manager_ptr)
{
    info.PluginName         = "E1.31 Receiver";
    info.PluginDescription  = "OpenRGB E1.31 Receiver Plugin";
    info.PluginLocation     = "TopTabBar";
    info.HasCustom          = false;
    info.SettingName        = "";
    info.PluginLabel        = new QLabel();

    /*-----------------------------------------------------*\
    | Set the label text                                    |
    \*-----------------------------------------------------*/
    info.PluginLabel->setText("E1.31 Receiver");

    /*-----------------------------------------------------*\
    | Save the arguments to Initialize based on what you    |
    | need for this plugin's functionality.  In this example|
    | we will need the Resource Manager to access the device|
    | list, so save the Resource Manager pointer locally.   |
    \*-----------------------------------------------------*/
    resource_manager = resource_manager_ptr;

    return info;
}

/*-----------------------------------------*\
| CreateGUI                                 |
|                                           |
|  This function must be present in all     |
|  OpenRGB plugins.  It creates the QWidget |
|  that represents the plugin tab's content |
\*-----------------------------------------*/
QWidget* OpenRGBPlugin::CreateGUI(QWidget* parent)
{
    /*-----------------------------------------------------*\
    | Create the main widget for this plugin tab            |
    \*-----------------------------------------------------*/
    QWidget* plugin_widget = new OpenRGBE131ReceiverDialog(resource_manager, parent);

    /*-----------------------------------------------------*\
    | The CreateGUI function must return the main widget    |
    \*-----------------------------------------------------*/
    return plugin_widget;
}
