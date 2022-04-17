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

