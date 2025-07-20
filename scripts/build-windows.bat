::---------------------------------------------------------::
:: OpenRGB E.131 Receiver Plugin Windows Build Script      ::
::                                                         ::
:: Sets up build environment for desired Qt version and    ::
:: build architecture, then builds OpenRGB E1.31 Receiver  ::
:: Plugin                                                  ::
::                                                         ::
:: Prerequisites:                                          ::
::  * Microsoft Visual Studio Build Tools                  ::
::  * OpenRGB Qt Packages                                  ::
::                                                         ::
:: Adam Honse (calcprogrammer1@gmail.com)   20 Jul 2025    ::
::---------------------------------------------------------::

::---------------------------------------------------------::
:: Parse arguments                                         ::
:: Format: build-windows.bat QT_VER MSVC_VER BITS          ::
:: Example: build-windows.bat 6.3.8 2022 64                ::
::---------------------------------------------------------::
@SET QT_VER=%1
@SET MSVC_VER=%2
@SET BITS=%3

::---------------------------------------------------------::
:: Set variables based on bits value                       ::
::---------------------------------------------------------::
@if %BITS% == 32 goto bits_32
@if %BITS% == 64 goto bits_64

:bits_32
@SET MSVC_ARCH=x86
@SET QT_PATH=
goto bits_done

:bits_64
@SET MSVC_ARCH=x64
@SET QT_PATH=_64
goto bits_done

:bits_done

::---------------------------------------------------------::
:: Add desired Qt version to PATH                          ::
::---------------------------------------------------------::
@SET "PATH=%PATH%;C:\Qt\%QT_VER%\msvc%MSVC_VER%%QT_PATH%\bin"

::---------------------------------------------------------::
:: Add jom to PATH                                         ::
::---------------------------------------------------------::
@SET "PATH=%PATH%;C:\Qt\jom"

::---------------------------------------------------------::
:: Run vcvarsall                                           ::
::---------------------------------------------------------::
@call "C:\Program Files (x86)\Microsoft Visual Studio\%MSVC_VER%\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" %MSVC_ARCH%

::---------------------------------------------------------::
:: Run qmake to configure the build                        ::
::---------------------------------------------------------::
qmake OpenRGBE131ReceiverPlugin.pro CONFIG-=debug_and_release CONFIG+=release

::---------------------------------------------------------::
:: Run jom to execute the build                            ::
::---------------------------------------------------------::
jom

::---------------------------------------------------------::
:: Rename the release folder                               ::
::---------------------------------------------------------::
move "release" "OpenRGBE131ReceiverPlugin Windows %BITS%-bit"
