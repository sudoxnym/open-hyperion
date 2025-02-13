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
MAJOR       = 0
MINOR       = 9
SUFFIX      = git

SHORTHASH   = $$system("git rev-parse --short=7 HEAD")
LASTTAG     = "release_"$$MAJOR"."$$MINOR
COMMAND     = "git rev-list --count "$$LASTTAG"..HEAD"
COMMITS     = $$system($$COMMAND)

VERSION_NUM = $$MAJOR"."$$MINOR"."$$COMMITS
VERSION_STR = $$MAJOR"."$$MINOR

VERSION_DEB = $$VERSION_NUM
VERSION_WIX = $$VERSION_NUM
VERSION_AUR = $$VERSION_NUM
VERSION_RPM = $$VERSION_NUM

equals(SUFFIX, "git") {
VERSION_STR = $$VERSION_STR"+ ("$$SUFFIX$$COMMITS")"
VERSION_DEB = $$VERSION_DEB"~git"$$SHORTHASH
VERSION_AUR = $$VERSION_AUR".g"$$SHORTHASH
VERSION_RPM = $$VERSION_RPM"^git"$$SHORTHASH
} else {
    !isEmpty(SUFFIX) {
VERSION_STR = $$VERSION_STR"+ ("$$SUFFIX")"
VERSION_DEB = $$VERSION_DEB"~"$$SUFFIX
VERSION_AUR = $$VERSION_AUR"."$$SUFFIX
VERSION_RPM = $$VERSION_RPM"^"$$SUFFIX
    }
}

message("VERSION_NUM: "$$VERSION_NUM)
message("VERSION_STR: "$$VERSION_STR)
message("VERSION_DEB: "$$VERSION_DEB)
message("VERSION_WIX: "$$VERSION_WIX)
message("VERSION_AUR: "$$VERSION_AUR)
message("VERSION_RPM: "$$VERSION_RPM)

#-----------------------------------------------------------------------------------------------#
# Automatically generated build information                                                     #
#-----------------------------------------------------------------------------------------------#
GIT_COMMIT_ID   = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ rev-parse HEAD)

#-----------------------------------------------------------------------------------------------#
# Inject vars in defines                                                                        #
#-----------------------------------------------------------------------------------------------#
DEFINES +=                                                                                      \
    VERSION_STRING=\\"\"\"$$VERSION_STR\\"\"\"                                                  \
    BUILDDATE_STRING=\\"\"\"$$BUILDDATE\\"\"\"                                                  \
    GIT_COMMIT_ID=\\"\"\"$$GIT_COMMIT_ID\\"\"\"                                                 \
    GIT_COMMIT_DATE=\\"\"\"$$GIT_COMMIT_DATE\\"\"\"                                             \
    GIT_BRANCH=\\"\"\"$$GIT_BRANCH\\"\"\"                                                       \
    LATEST_BUILD_URL=\\"\"\"$$LATEST_BUILD_URL\\"\"\"                                           \


#-----------------------------------------------------------------------------------------------#
# OpenRGB Plugin SDK                                                                            #
#-----------------------------------------------------------------------------------------------#
INCLUDEPATH +=                                                                                  \
    OpenRGB                                                                                     \
    OpenRGB/RGBController                                                                       \
    OpenRGB/dependencies/json                                                                   \
    OpenRGB/qt                                                                                  \
    OpenRGB/i2c_smbus                                                                           \
    OpenRGB/net_port                                                                            \

HEADERS +=                                                                                      \
    OpenRGB/Colors.h                                                                            \
    OpenRGB/OpenRGBPluginInterface.h                                                            \
    OpenRGB/ResourceManagerInterface.h                                                          \

SOURCES +=                                                                                      \
    OpenRGB/RGBController/RGBController.cpp                                                     \
    OpenRGB/RGBController/RGBController_Network.cpp                                             \
    OpenRGB/NetworkServer.cpp                                                                   \
    OpenRGB/NetworkClient.cpp                                                                   \
    OpenRGB/NetworkProtocol.cpp                                                                 \
    OpenRGB/LogManager.cpp                                                                      \
    OpenRGB/net_port/net_port.cpp                                                               \

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

RESOURCES +=                                                                                    \
    resources.qrc
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
}

