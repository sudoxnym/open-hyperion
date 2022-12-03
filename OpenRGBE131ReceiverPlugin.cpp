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

OpenRGBPluginInfo OpenRGBPlugin::GetPluginInfo()
{
    OpenRGBPluginInfo info;

    info.Name           = "OpenRGB E1.31 Receiver Plugin";
    info.Description    = "Receive E1.31 lighting control data from external applications to control OpenRGB devices";
    info.Version        = VERSION_STRING;
    info.Commit         = GIT_COMMIT_ID;
    info.URL            = "https://gitlab.com/OpenRGBDevelopers/OpenRGBE131ReceiverPlugin";

    info.Location       = OPENRGB_PLUGIN_LOCATION_TOP;
    info.Label          = "E1.31 Receiver";
    info.TabIcon.load(":/OpenRGBE131ReceiverPlugin.png");
    info.Icon.load(":/OpenRGBE131ReceiverPlugin.png");

    return(info);
}

unsigned int OpenRGBPlugin::GetPluginAPIVersion()
{
    return(OPENRGB_PLUGIN_API_VERSION);
}

void OpenRGBPlugin::Load(bool dark_theme, ResourceManager* resource_manager_ptr)
{
    /*-----------------------------------------------------*\
    | Save the arguments to Initialize based on what you    |
    | need for this plugin's functionality.  In this example|
    | we will need the Resource Manager to access the device|
    | list, so save the Resource Manager pointer locally.   |
    \*-----------------------------------------------------*/
    resource_manager = resource_manager_ptr;
}

QWidget* OpenRGBPlugin::GetWidget()
{
    /*-----------------------------------------------------*\
    | Create the main widget for this plugin tab            |
    \*-----------------------------------------------------*/
    QWidget* plugin_widget = new OpenRGBE131ReceiverDialog(resource_manager, nullptr);

    /*-----------------------------------------------------*\
    | The CreateGUI function must return the main widget    |
    \*-----------------------------------------------------*/
    return plugin_widget;
}

QMenu* OpenRGBPlugin::GetTrayMenu()
{
    return(nullptr);
}

void OpenRGBPlugin::Unload()
{

}
