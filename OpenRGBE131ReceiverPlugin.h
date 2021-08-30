/*-----------------------------------------*\
|  OpenRGBPlugin.h                          |
|                                           |
|  OpenRGB Plugin template with example     |
|                                           |
|  herosilas12 (CoffeeIsLife)   12/11/2020  |
|  Adam Honse (CalcProgrammer1) 1/5/2021    |
\*-----------------------------------------*/

#pragma once

#include "OpenRGBPluginInterface.h"

#include <QObject>
#include <QString>
#include <QtPlugin>
#include <QWidget>
#include <QLabel>

class OpenRGBPlugin : public QObject, public OpenRGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID OpenRGBPluginInterface_IID)
    Q_INTERFACES(OpenRGBPluginInterface)

public:
    virtual                    ~OpenRGBPlugin() {}

    /*-------------------------------------------------------------------------------------------------*\
    | Plugin Information                                                                                |
    \*-------------------------------------------------------------------------------------------------*/
    virtual OpenRGBPluginInfo   GetPluginInfo()                                                     override;
    virtual unsigned int        GetPluginAPIVersion()                                               override;

    /*-------------------------------------------------------------------------------------------------*\
    | Plugin Functionality                                                                              |
    \*-------------------------------------------------------------------------------------------------*/
    virtual void                Load(bool dark_theme, ResourceManager* resource_manager_ptr)        override;
    virtual QWidget*            GetWidget()                                                         override;
    virtual QMenu*              GetTrayMenu()                                                       override;
    virtual void                Unload()                                                            override;
private:
    ResourceManager*        resource_manager;
};
