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
    widgets                                                                                     \

DEFINES += ORGBEXAMPLEPLUGIN_LIBRARY
TEMPLATE = lib

#-----------------------------------------------------------------------------------------------#
# Build Configuration                                                                           #
#-----------------------------------------------------------------------------------------------#
CONFIG +=                                                                                       \
    plugin                                                                                      \
    silent                                                                                      \

#-----------------------------------------------------------------------------------------------#
# Application Configuration                                                                     #
#-----------------------------------------------------------------------------------------------#
MAJOR           = 0
MINOR           = 9
REVISION        = 1
PLUGIN_VERSION  = $$MAJOR"."$$MINOR$$REVISION

#-----------------------------------------------------------------------------------------------#
# Automatically generated build information                                                     #
#-----------------------------------------------------------------------------------------------#
GIT_COMMIT_ID   = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ rev-parse HEAD)

#-----------------------------------------------------------------------------------------------#
# Inject vars in defines                                                                        #
#-----------------------------------------------------------------------------------------------#
DEFINES +=                                                                                      \
    VERSION_STRING=\\"\"\"$$PLUGIN_VERSION\\"\"\"                                               \
    GIT_COMMIT_ID=\\"\"\"$$GIT_COMMIT_ID\\"\"\"

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

FORMS +=                                                                                        \
    OpenRGBE131ReceiverDialog.ui

#-----------------------------------------------------------------------------------------------#
# OpenRGB Plugin SDK                                                                            #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    OpenRGB/                                                                                    \
    OpenRGB/dependencies/json                                                                   \
    OpenRGB/i2c_smbus                                                                           \
    OpenRGB/net_port                                                                            \
    OpenRGB/RGBController                                                                       \
    OpenRGB/dependencies/hidapi                                                                 \
    OpenRGB/hidapi_wrapper                                                                      \

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
win32:CONFIG += c++17

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
# Linux-specific Configuration                                                                  #
#-----------------------------------------------------------------------------------------------#
unix:!macx {
    LIBS += -lopenal
    QMAKE_CXXFLAGS += -std=c++17 -Wno-psabi
    target.path=$$PREFIX/lib/openrgb/plugins/
    INSTALLS += target
}

#-----------------------------------------------------------------------------------------------#
# MacOS-specific Configuration                                                                  #
#-----------------------------------------------------------------------------------------------#
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

macx: {
    CONFIG += c++17
    LIBS += -framework OpenAL
}

RESOURCES +=                                                                                    \
    resources.qrc
