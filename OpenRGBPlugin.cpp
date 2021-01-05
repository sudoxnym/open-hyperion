/*-----------------------------------------*\
|  OpenRGBPlugin.cpp                        |
|                                           |
|  OpenRGB Plugin template with example     |
|                                           |
|  herosilas12 (CoffeeIsLife)   12/11/2020  |
|  Adam Honse (CalcProgrammer1) 1/5/2021    |
\*-----------------------------------------*/

#include "OpenRGBPlugin.h"

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
    info.PluginName         = "OpenRGB Plugin Template";
    info.PluginDescription  = "An example plugin for OpenRGB";
    info.PluginLocation     = "InformationTab";
    info.HasCustom          = false;
    info.SettingName        = "";
    info.PluginLabel        = new QLabel();

    /*-----------------------------------------------------*\
    | Set the label text                                    |
    \*-----------------------------------------------------*/
    info.PluginLabel->setText("OpenRGB Example Plugin");

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
    QWidget* plugin_widget = new QWidget(parent);

    /*-----------------------------------------------------*\
    | In this example, we will create a label showing the   |
    | number of RGBController devices.  This will be shown  |
    | in a QLabel, updated at 1Hz by a background thread.   |
    \*-----------------------------------------------------*/
    plugin_label = new QLabel(plugin_widget);

    /*-----------------------------------------------------*\
    | With the label created, start the worker thread       |
    \*-----------------------------------------------------*/
    TimerThread = new std::thread(&OpenRGBPlugin::TimerThreadFunction, this);

    /*-----------------------------------------------------*\
    | The CreateGUI function must return the main widget    |
    \*-----------------------------------------------------*/
    return plugin_widget;
}

/*-----------------------------------------*\
| TimerThreadFunction                       |
|                                           |
|  This function is part of the example code|
|  and is not a required part of an OpenRGB |
|  plugin.  This function is an example of  |
|  how a plugin can run a background thread |
|  and interact with the Resource Manager   |
\*-----------------------------------------*/
void OpenRGBPlugin::TimerThreadFunction()
{
    /*-----------------------------------------------------*\
    | Begin infinite loop                                   |
    \*-----------------------------------------------------*/
    while(1)
    {
        /*-------------------------------------------------*\
        | Print the number of devices to a string           |
        \*-------------------------------------------------*/
        std::string text;

        text.append("Number of devices detected: ");
        text.append(std::to_string(resource_manager->GetRGBControllers().size()));
        text.append("\r\n");

        /*-------------------------------------------------*\
        | Update the label                                  |
        \*-------------------------------------------------*/
        plugin_label->setText(QString::fromStdString(text));

        /*-------------------------------------------------*\
        | Sleep for 1 second                                |
        \*-------------------------------------------------*/
        Sleep(1000);
    }
}
