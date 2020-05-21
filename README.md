# CachingBuddy
ESP8266-based low-tech caching device.

Intended for Geocaching as group activity.

The texts are in german, sorry for that, maybe I will create a translated version aswell.

## Features
* Display Cache details on OLED display incl. course/distance to cache
* LED-Ring compass towards cache
* Cache itearation with support up to 255 caches
* Management via HTML-Interface. Access via QR-URL possible

> Feature Details to be done

## Components

### Hardware
* ESP8266 on a WeMos D1 Mini board
* GPS Sensor: Beitian BN-220
* Compass: 24-LED Ring CJMCV-2812-24
* Display: 128 x 64 Pixel 1.3" OLED with SH1106 I2C controller
* Buttons: 12 x 12 x 7.3mm SMD buttons
* Mini DC-DC step down converter (Using 2 18650 LiIon Batteries)

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
