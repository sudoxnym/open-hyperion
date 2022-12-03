#-----------------------------------------------------------------------------------------------#
# OpenRGB E1.31 Receiver Plugin QMake Project                                                   #
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
# Automatically generated build information                                                     #
#-----------------------------------------------------------------------------------------------#
PLUGIN_VERSION  = 0.8
GIT_COMMIT_ID   = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ rev-parse HEAD)

DEFINES +=                                                                                      \
    VERSION_STRING=\\"\"\"$$PLUGIN_VERSION\\"\"\"                                               \
    GIT_COMMIT_ID=\\"\"\"$$GIT_COMMIT_ID\\"\"\"

#-----------------------------------------------------------------------------------------------#
# Application Configuration                                                                     #
#-----------------------------------------------------------------------------------------------#
TEMPLATE = lib
DEFINES += ORGBEXAMPLEPLUGIN_LIBRARY

CONFIG += c++11

#-----------------------------------------------------------------------------------------------#
# Plugin Project Files                                                                          #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    dependencies/libe131/src/                                                                   \

SOURCES +=                                                                                      \
    dependencies/libe131/src/e131.c                                                             \
    OpenRGBE131ReceiverDialog.cpp                                                               \
    OpenRGBE131ReceiverPlugin.cpp                                                               \

HEADERS +=                                                                                      \
    dependencies/libe131/src/e131.h                                                             \
    OpenRGBE131ReceiverDialog.h                                                                 \
    OpenRGBE131ReceiverPlugin.h                                                                 \

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
# Windows-specific Configuration                                                                #
#-----------------------------------------------------------------------------------------------#
win32:contains(QMAKE_TARGET.arch, x86_64) {
    LIBS +=                                                                                     \
        -lws2_32                                                                                \
}

win32:contains(QMAKE_TARGET.arch, x86) {
    LIBS +=                                                                                     \
        -lws2_32                                                                                \
}

win32:DEFINES -=                                                                                \
    UNICODE

win32:DEFINES +=                                                                                \
    _MBCS                                                                                       \
    WIN32                                                                                       \
    _CRT_SECURE_NO_WARNINGS                                                                     \
    _WINSOCK_DEPRECATED_NO_WARNINGS                                                             \
    WIN32_LEAN_AND_MEAN

#-----------------------------------------------------------------------------------------------#
# Default rules for deployment.                                                                 #
#-----------------------------------------------------------------------------------------------#
unix:!macx {
    target.path = /usr/lib
}

!isEmpty(target.path): INSTALLS += target

FORMS += \
    OpenRGBE131ReceiverDialog.ui

#-----------------------------------------------------------------------#
# MacOS-specific Configuration                                          #
#-----------------------------------------------------------------------#
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

macx: {
    CONFIG += c++17
    LIBS += -framework OpenAL
}

RESOURCES += \
    resources.qrc
