# CachingBuddy
ESP8266-based low-tech caching device.

Intended for Geocaching as group activity.

The texts are in german only currently, sorry for that, but I extracted all tests to a separate header file.

## Features
* Display Cache details on OLED display incl. course/distance to cache
* LED-Ring compass towards cache
* Cache itearation with support up to 255 caches
* Management via HTML-Interface. Access via QR-URL possible

### Geocaches in general
The **CachingBuddy** uses the *SPIFF* capabilites of the ESP8266 to store the geocaches in the default GPX-format, as they can be downloaded from geocaching.com. The maximum supported number of Caches on the device has been set to **128** as more do not seem to make sense for a single tour.

The implementation allows even to use Mystery-Caches (As their coordinates are updated after solving them) and muti-caches:

_Multi-Caches_ have multiple waypoints. **CachingBuddy** has been implemented to support up to **32** waypoints per cache.

### UI & Buttons

The UI is split into 3 frames with different contents and meanings.
Due to the separation of topics on every frame, 2 buttons attached to the **CachingBuddy** are anough to allow every action:

1. The first button changes the current frame in a ring buffer
2. The second button executes the frame-depentent action. These are explained in the frame descriptions below.

#### Frame 1: Overview
On stratup, **CachingBuddy** loads the geocache from the first available GPX file. The main screen is stuffed with all available information of the current cache, the current cache's currelntly selected waypoint and the current search. The data are in detail:

* Geocode
* Container Size and D/T ratings
* Name of Cache _or_ the selected additional waypoints
* The current waypoint number and overall waypoint count of the cache
* Distance and course towards the selected waypoint
* Current (HDOP)[https://en.wikipedia.org/wiki/Dilution_of_precision_(navigation)] value as human-readable string and the number of currently used satellites
* > Requested: Type of cache

***Option Button***: The option button iterates the currently selected waypoint in a ring buffer.

#### Frame 2: GPX List

The Geocaches are available in a list that can be iterated. **CachingBuddy** loads them on startup.
The list is split into pages of 4 (due to the little space) and highlights the currently loaded cache.

***Option Button***: The option button iterates the currently selected waypoint in a ring buffer.

#### Frame 3: Wifi Settings

**CachingBuddy** Provides a HTML-Administration which is delivered through an onboard WiFi-AccessPoint. This is disabled by deafult. This page shows the status and details of this Administration.

If activated the SSID of the AccessPoint with a QR-code in (MECARD-Syntax)[https://github.com/zxing/zxing/wiki/Barcode-Contents#wi-fi-network-config-android-ios-11]  is displayed in the upper half and in the lower half the IP address of **CachingBuddy** is displayed, again with a QR-code of the URL of the Administration.

This allows to connect to the administration by simply scanning 2 QR-codes

***Option Button***: The option button toggles the WiFi administration server and the hardware interface.

## Components

### Hardware
* ESP8266 on a WeMos D1 Mini board
* GPS Sensor: Beitian BN-220
* Compass: 24-LED Ring CJMCV-2812-24
* Display: 128 x 64 Pixel 1.3" OLED with SH1106 I2C controller
* Buttons: 12 x 12 x 7.3mm SMD buttons
* Mini DC-DC step down converter (Using 2 Panasonic NCR18650B LiIon Batteries)

The latest Version has introduced some energy optimizations, which allows operation in default mode (WiFi server deactivated) as low as **< 90mA**, which leads to a potential runtime of approx. **75hrs** with my setup (depending on the batteries you use).

#### Wiring Diagram

### Software
* Arduino with default libraries, e.g. `Adafruit_NeoPixel`
* GPS: [TinyGPS++](https://github.com/mikalhart/TinyGPSPlus)
* Display: [ThingPulse OLED SSD1306 (ESP8266/ESP32/Mbed-OS)](https://github.com/ThingPulse/esp8266-oled-ssd1306)
* GPX Reading with a [TinyXML Port](https://github.com/adafruit/TinyXML)
* [ESP8266 Libraries](https://github.com/esp8266/Arduino) e.g. ESP8266WiFi, ESP8266WebServer
* [QRCode Library](https://github.com/ricmoo/QRCode)

## Thanks to
* Contributirs to Arduino and ESP8266 libraries
* @mikalhart from [TinyGPS++](https://github.com/mikalhart/TinyGPSPlus)
* @squix78, @FWeinb and @helmut64 from [ThingPulse](https://github.com/ThingPulse/)
* @adamvr for his [TinyXML port](https://github.com/adafruit/TinyXML)
* @ricmoo for his [QRCode lib](https://github.com/ricmoo/QRCode)
