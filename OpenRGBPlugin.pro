#-----------------------------------------------------------------------------------------------#
# OpenRGB Plugin Template QMake Project                                                         #
#                                                                                               #
#   herosilas12 (CoffeeIsLife)                          12/11/2020                              #
#   Adam Honse (CalcProgrammer1)                        1/5/2021                                #
#-----------------------------------------------------------------------------------------------#

#-----------------------------------------------------------------------------------------------#
# Qt Configuration                                                                              #
#-----------------------------------------------------------------------------------------------#
QT +=                                                                                           \
    core                                                                                        \
    gui                                                                                         \

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#-----------------------------------------------------------------------------------------------#
# Application Configuration                                                                     #
#-----------------------------------------------------------------------------------------------#

TEMPLATE = lib
DEFINES += ORGBEXAMPLEPLUGIN_LIBRARY

CONFIG += c++11

#-----------------------------------------------------------------------------------------------#
# Plugin Project Files                                                                          #
#-----------------------------------------------------------------------------------------------#
SOURCES +=                                                                                      \
    OpenRGBPlugin.cpp                                                                           \

HEADERS +=                                                                                      \
    OpenRGBPlugin.h                                                                             \

#-----------------------------------------------------------------------------------------------#
# OpenRGB Plugin SDK                                                                            #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    OpenRGB/                                                                                    \
    OpenRGB/dependencies/json                                                                   \
    OpenRGB/i2c_smbus                                                                           \
    OpenRGB/net_port                                                                            \
    OpenRGB/RGBController                                                                       \

HEADERS +=                                                                                      \
    OpenRGB/NetworkClient.h                                                                     \
    OpenRGB/NetworkProtocol.h                                                                   \
    OpenRGB/NetworkServer.h                                                                     \
    OpenRGB/OpenRGBPluginInterface.h                                                            \
    OpenRGB/ProfileManager.h                                                                    \
    OpenRGB/ResourceManager.h                                                                   \
    OpenRGB/SettingsManager.h                                                                   \
    OpenRGB/dependencies/json/json.hpp                                                          \
    OpenRGB/i2c_smbus/i2c_smbus.h                                                               \
    OpenRGB/net_port/net_port.h                                                                 \
    OpenRGB/RGBController/RGBController.h                                                       \

#-----------------------------------------------------------------------------------------------#
# Default rules for deployment.                                                                 #
#-----------------------------------------------------------------------------------------------#
unix {
    target.path = /usr/lib
}

!isEmpty(target.path): INSTALLS += target
