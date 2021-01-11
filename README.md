# OpenRGB E1.31 Receiver Plugin

This is a plugin for OpenRGB that adds E1.31 integration.  E1.31 is an industry-standard lighting protocol used for all sorts of lighting control and is supported by a wide variety of lighting control software.  E1.31 is often used for sequenced light shows such as Christmas light displays with music.  This plugin allows you to control all of your OpenRGB-supported hardware from these lighting control programs by turning your OpenRGB setup into an E1.31 receiver.

# Building

Build the plugin using QT Creator or QMake using the .pro file.

# Installation

Copy the plugin to your OpenRGB plugins directory.

* Windows: %APPDATA%\OpenRGB\plugins
* Linux: ~/.config/OpenRGB/plugins

# Usage

Once installed, you will have an E1.31 Receiver tab in OpenRGB.  Select this tab.  Once detection is complete, the list of universes should be populated.  After this, you can simply click the Start Receiver button and OpenRGB is ready to receive E1.31 packets.

# Screenshot

![image](/uploads/7aa973dfa45a98ffa394d748a6125d44/image.png)
