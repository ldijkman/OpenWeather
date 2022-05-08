<p><a href="https://stand-with-ukraine.pp.ua/#" rel="nofollow"><img src="https://raw.githubusercontent.com/vshymanskyy/StandWithUkraine/main/banner2-direct.svg" alt="Stand With Ukraine"></a></p>

### https://russiaforsale.org/

---

trying to change the settings to a littlefs file config.txt

so that you electra can edit the littlefs config.txt file from ace js inbrowser web cloud editor

nice add on for electra touch tft https://ldijkman.github.io/randomnerd_esp32_wifi_manager/Touch_Electra-Electra_Touch/

program / install it on your ESP8266 12E,12F from Browser

touch Electra

playing with the files at https://github.com/ldijkman/OpenWeather/tree/master/examples/TFT_eSPI_OpenWeather_LittleFS

## changed bmp icons to jpg that saves a lot of littlefs size
- linux command batch convert bmp to jpg => mogrify -format jpg *.bmp
- /data littlefs directory something => was 1086kb bmp => now 175kb jpg
- do not see any problems with the change to jpg icon images yet
- think its possible to save more littlefs space if images are cropped to content
- but i am happy for now it will fit into Electra
- 
---

<img src="https://github.com/ldijkman/randomnerd_esp32_wifi_manager/raw/main/docs/Touch_Electra-Electra_Touch/img_1_1651851213226.jpg" width="50%">

https://ldijkman.github.io/randomnerd_esp32_wifi_manager/Touch_Electra-Electra_Touch/

<img src="https://github.com/ldijkman/randomnerd_esp32_wifi_manager/raw/main/docs/Touch_Electra-Electra_Touch/img_2_1651851229852.jpg" width="50%">

https://ldijkman.github.io/randomnerd_esp32_wifi_manager/Touch_Electra-Electra_Touch/


---
---
---
---


Original by: http://blog.squix.org
- https://blog.squix.org/2017/07/new-weatherstation-color-version-published.html

Adapted by: Bodmer

forked from Bodmer/OpenWeather https://github.com/Bodmer/OpenWeather
---

Touch Electra, TFT_eSPI Touch, install from browser on ESP8266 12E / 12F
https://github.com/ldijkman/randomnerd_esp32_wifi_manager/tree/main/ESP8266-TFT_eSPI

---


# RP2040 Nano Connect, ESP8266 and ESP32 OpenWeather client

Arduino client library for https://openweathermap.org/

Collects current weather plus daily forecasts.

Requires the JSON parse library here:
https://github.com/Bodmer/JSON_Decoder

The OpenWeather_Test example sketch sends collected data to the Serial port for API test. It does not not require a TFT screen and works with ESPX ESP8266.

The TFT_eSPI_OpenWeather_LittleFS example works with the RP2040 Nano Connect, ESP32 and ESP8266 and uses LittleFS, it displays the weather data on a TFT screen.

The TFT_eSPI_Weather example works with the ESP8266 and ESP32 only and uses SPIFFS, it displays the weather data on a TFT screen.

The RP2040 Nano Connect must be used with Earle Philhower's board package:
https://github.com/earlephilhower/arduino-pico

These examples use anti-aliased fonts and newly created icons:

![Weather isons](https://i.imgur.com/luK7Vcj.jpg)

Latest screen grabs:

![TFT screenshot 1](https://i.imgur.com/ORovwNY.png)

forked from Bodmer/OpenWeather https://github.com/Bodmer/OpenWeather

Original by: http://blog.squix.org
- https://blog.squix.org/2017/07/new-weatherstation-color-version-published.html

Adapted by: Bodmer
