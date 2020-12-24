#pragma once

#include "ORGBPluginInterface.h"
#include "Dependencies/ResourceManager.h"

#include <QObject>
#include <QString>
#include <QtPlugin>
#include "QWidget"
#include "QLabel"
#include "QPushButton"
#include "QDialog"
#include "QAction"

class ORGBPlugin : public QObject, public ORGBPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ORGBPluginInterface_IID)
    Q_INTERFACES(ORGBPluginInterface)

public:
    virtual                 ~ORGBPlugin() {};

    PluginInfo              PInfo;

    virtual PluginInfo      DefineNeeded() override;

    virtual PluginInfo      init(json Settings , bool DarkTheme) override;

    virtual QWidget         *CreateGUI(QWidget *Parent) override;
private slots:
    void on_ExampleButton_clicked();
};
