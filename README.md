# OpenRGB E1.31 Receiver Plugin

This is a plugin for OpenRGB that adds E1.31 integration.  E1.31 is an industry-standard lighting protocol used for all sorts of lighting control and is supported by a wide variety of lighting control software.  E1.31 is often used for sequenced light shows such as Christmas light displays with music.  This plugin allows you to control all of your OpenRGB-supported hardware from these lighting control programs by turning your OpenRGB setup into an E1.31 receiver.
# Stable (0.9)

* [Windows 64](https://gitlab.com/OpenRGBDevelopers/OpenRGBE131ReceiverPlugin/-/jobs/4632325324/artifacts/download)
* [Linux 64](https://gitlab.com/OpenRGBDevelopers/OpenRGBE131ReceiverPlugin/-/jobs/4632325321/artifacts/download)
* [MacOS ARM64](https://gitlab.com/OpenRGBDevelopers/OpenRGBE131ReceiverPlugin/-/jobs/4632325325/artifacts/download)
* [MacOS Intel](https://gitlab.com/OpenRGBDevelopers/OpenRGBE131ReceiverPlugin/-/jobs/4632421480/artifacts/download)

# Building

Build the plugin using QT Creator or QMake using the .pro file.

# Installation

Copy the plugin to your OpenRGB plugins directory.

* Windows: %APPDATA%\OpenRGB\plugins
* Linux: ~/.config/OpenRGB/plugins

# Usage

Once installed, you will have an E1.31 Receiver tab in OpenRGB.  Select this tab.  Once detection is complete, the list of controllers on the left should be populated.  You must map controllers to universes.  You may do this manually by clicking the Add Universe button to create a universe, then selecting both the universe and a controller from the left, then clicking Add Controller to add the controller to the universe.  You can also automatically map the controllers by clicking the Auto-Map button, which assigns one controller to each universe, automatically creating additoinal universes for controllers that do not fit within a single universe.  After the mapping is set up, you can simply click the Start Receiver button and OpenRGB is ready to receive E1.31 packets.  You can save the mapping with the Save Map button so that you can load it back next time with the Load Map button.

# Tested E1.31 software

* Vixen 3 (http://www.vixenlights.com/)
* Jinx! (http://www.live-leds.de/downloads/)
* sACNView (https://sacnview.org/)
* QLC+ (https://www.qlcplus.org/index.html)

# Screenshot

![image](/uploads/e91269ac93bb69ce40955babf8e7af7e/image.png)
