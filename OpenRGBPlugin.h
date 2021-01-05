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
    virtual                        ~OpenRGBPlugin() {};

    virtual OpenRGBPluginInfo       Initialize(bool dark_theme, ResourceManager* resource_manager_ptr) override;

    virtual QWidget                *CreateGUI(QWidget* parent) override;

    void TimerThreadFunction();

private:
    ResourceManager*        resource_manager;
    QLabel*                 plugin_label;

    std::thread* TimerThread;
};
