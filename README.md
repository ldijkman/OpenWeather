# ESP8266 and ESP32 OpenWeather client

(Note: the ESP32 Arduino board package 1.0.4 works fine, but the more recent 1.0.5 does not. Reason is unknown.)

Arduino client library for https://openweathermap.org/

Collects current weather plus daily forecasts.

Requires the JSON parse library here:
https://github.com/Bodmer/JSON_Decoder

The OpenWeather_Test example sketch sends collected data to the Serial port for API test. It does not not require a TFT screen.

The TFT_eSPI_Weather example works with the ESP8266 and ESP32, it displays the weather data on a TFT screen.  These examples use anti-aliased fonts and newly created icons:

![Weather isons](https://i.imgur.com/luK7Vcj.jpg)

Latest screen grabs:

![TFT screenshot 1](https://i.imgur.com/ORovwNY.png)

