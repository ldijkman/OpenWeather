

//*************************************************************************
//
// https://github.com/ldijkman/OpenWeather
// https://github.com/ldijkman/randomnerd_esp32_wifi_manager/tree/main/ESP8266-TFT_eSPI
// https://ldijkman.github.io/randomnerd_esp32_wifi_manager/Touch_Electra-Electra_Touch/
//
//  4 electa read some settings from /data littlefs /config.txt
// so that you can change it in future from ace js inbrowser cloud web editor
// so that it can/will become part of electra touch tft browser install ESP8266 4mbyte ESP12E ESP12F
// luberth
//************************************************************************

//  Example from OpenWeather library: https://github.com/Bodmer/OpenWeather
//  Adapted by Bodmer to use the TFT_eSPI library:  https://github.com/Bodmer/TFT_eSPI

//  This sketch is compatible with  ESP8266 12E 12F 32mbit/4MByte

//                           >>>  IMPORTANT  <<<
//         Modify setup in All_Settings.h tab to configure your location etc
//         luberth => some data move to /config.txt littlefs /data
//         do not want it hardcoded  ace js inbrowser cloudeditor should edit littlefs config.txt or wifimanager 


//                >>>  EVEN MORE IMPORTANT TO PREVENT CRASHES <<<
//>>>>>>  For ESP8266 set LittleFS to at least 2Mbytes before uploading files  <<<<<<
// luberth => well i want to make this a webbrowser install on ESP8266 / program esp from webbrowser
// so that wil take care of littlefs size settings

//  ESP8266 pin connections to the TFT are defined in the TFT_eSPI library.
// luberth => i use  these for Electra https://github.com/ldijkman/randomnerd_esp32_wifi_manager/tree/main/ESP8266-TFT_eSPI

//  Original by Daniel Eichhorn, see license at end of file.
//  See more at http://blog.squix.ch

//#define SERIAL_MESSAGES // For serial output weather reports


// This sketch uses font files created from the Noto family of fonts as bitmaps
// generated from these fonts may be freely distributed:
// https://www.google.com/get/noto/

// Json streaming parser (do not use IDE library manager version) to use is here:
// https://github.com/Bodmer/JSON_Decoder


// luberth => image db files can be removed to save littlefs size i think
// could bmp be jpg to save diskspace?
// view image db files online with https://thumbsdb.herokuapp.com/

#include <FS.h>
#include <LittleFS.h>

#define AA_FONT_SMALL "fonts/NSBold15" // 15 point Noto sans serif bold
#define AA_FONT_LARGE "fonts/NSBold36" // 36 point Noto sans serif bold
/***************************************************************************************
**                          Load the libraries and settings
***************************************************************************************/
#include <Arduino.h>

#include <SPI.h>
#include <TFT_eSPI.h> // https://github.com/Bodmer/TFT_eSPI

// Additional functions
#include "GfxUi.h"          // Attached to this sketch


#include <ESP8266WiFi.h>

// check All_Settings.h for adapting to your needs
#include "All_Settings.h"  // part of config is in config.txt

#include <JSON_Decoder.h> // https://github.com/Bodmer/JSON_Decoder

#include <OpenWeather.h>  // Latest here: https://github.com/Bodmer/OpenWeather

#include "NTP_Time.h"     // Attached to this sketch, see that tab for library needs

/***************************************************************************************
**                          Define the globals and class instances
***************************************************************************************/

TFT_eSPI tft = TFT_eSPI();             // Invoke custom library

OW_Weather ow;      // Weather forecast library instance

OW_current *current; // Pointers to structs that temporarily holds weather data
OW_hourly  *hourly;  // Not used
OW_daily   *daily;

boolean booted = true;


// next used in /config.txt
String  api_key;
// Set the forecast longitude and latitude to at least 4 decimal places
String latitude;// =  "52.735434"; // 90.0000 to -90.0000 negative for Southern hemisphere
String longitude;// = "5.179017"; // 180.000 to -180.000 negative for West



GfxUi ui = GfxUi(&tft); // Jpeg and bmpDraw functions TODO: pull outside of a class

long lastDownloadUpdate = millis();



/***************************************************************************************
**                          Declare prototypes
***************************************************************************************/
void updateData();
void drawProgress(uint8_t percentage, String text);
void drawTime();
void drawCurrentWeather();
void drawForecast();
void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex);
const char* getMeteoconIcon(uint16_t id, bool today);
void drawAstronomy();
void drawSeparator(uint16_t y);
void fillSegment(int x, int y, int start_angle, int sub_angle, int r, unsigned int colour);
String strDate(time_t unixTime);
String strTime(time_t unixTime);
void printWeather(void);
int leftOffset(String text, String sub);
int rightOffset(String text, String sub);
int splitIndex(String text);

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // Return 1 to decode next block
  return 1;
}

/***************************************************************************************
**                          Setup
***************************************************************************************/
void setup() {
  Serial.begin(115200);

  tft.begin();
  tft.setRotation(1); // For 320x480 screen
  tft.fillScreen(TFT_BLACK);

  while (!LittleFS.begin()) {
    Serial.println("Flash FS initialisation failed!");
    // while (1) yield(); // Stay here twiddling thumbs waiting
    delay(100);
  }
  Serial.println("\nFlash FS available!");
  if (LittleFS.exists("/config.txt")   == true) {
    File file = LittleFS.open("/config.txt", "r");
    delay(100);
    api_key = file.readStringUntil('\n');
    latitude = file.readStringUntil('\n');
    longitude = file.readStringUntil('\n');


    Serial.print("-"); Serial.print(api_key); Serial.print("-");
    Serial.println("...");
    Serial.print("-"); Serial.print(latitude); Serial.print("-");
    Serial.println("...");
    Serial.print("-"); Serial.print(longitude); Serial.print("-");
    Serial.println("...");

  }



  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);
  TJpgDec.setSwapBytes(true); // May need to swap the jpg colour bytes (endianess)

  // Draw splash screen
  // if (LittleFS.exists("/splash/OpenWeather.jpg")   == true) {
  TJpgDec.drawFsJpg(0, 0, "/electra_ohm_law.jpg", LittleFS);
  // }

  delay(2500);

  // Clear bottom section of screen
  tft.fillRect(0, 206, 240, 320 - 206, TFT_BLACK);

  tft.loadFont(AA_FONT_SMALL, LittleFS);
  tft.setTextDatum(BC_DATUM); // Bottom Centre datum
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);

  tft.drawString("Original by: blog.squix.org", 120, 260);
  tft.drawString("Adapted by: Bodmer", 120, 280);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);

  delay(2000);

  tft.fillRect(0, 206, 240, 320 - 206, TFT_BLACK);

  tft.drawString("Connecting to WiFi", 120, 240);
  tft.setTextPadding(240); // Pad next drawString() text to full width to over-write old text


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  tft.drawString("Connecting to KaRo", 120, 240);
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(i);
    delay(1000);
    i++;
    if (i > 30) {
      tft.drawString("Connecting Bangert", 120, 240);
      Serial.println("");
      WiFi.begin("Bangert-30-Andijk", "ikwilerin");   // connect to my wifi if karo did not work in 30 seconds
      i = 0;
    }
  }
  Serial.println("");

  tft.setTextDatum(BC_DATUM);
  tft.setTextPadding(240); // Pad next drawString() text to full width to over-write old text
  tft.drawString(" ", 120, 220);  // Clear line above using set padding width
  tft.drawString("Fetching weather data...", 120, 240);

  // Fetch the time
  udp.begin(localPort);
  syncTime();

  tft.unloadFont();

  ow.partialDataSet(true); // Collect a subset of the data available
}

/***************************************************************************************
**                          Loop
***************************************************************************************/
void loop() {

  // Check if we should update weather information
  if (booted || (millis() - lastDownloadUpdate > 1000UL * UPDATE_INTERVAL_SECS))
  {
    updateData();
    lastDownloadUpdate = millis();
  }

  // If minute has changed then request new time from NTP server
  if (booted || minute() != lastMinute)
  {
    // Update displayed time first as we may have to wait for a response
    drawTime();
    lastMinute = minute();

    // Request and synchronise the local clock
    syncTime();

  }

  booted = false;
}

/***************************************************************************************
**                          Fetch the weather data  and update screen
***************************************************************************************/
// Update the Internet based information and update screen
void updateData() {
  // booted = true;  // Test only
  // booted = false; // Test only

  if (booted) drawProgress(20, "Updating time...");
  else fillSegment(22, 22, 0, (int) (20 * 3.6), 16, TFT_NAVY);

  if (booted) drawProgress(50, "Updating conditions...");
  else fillSegment(22, 22, 0, (int) (50 * 3.6), 16, TFT_NAVY);

  // Create the structures that hold the retrieved weather
  current = new OW_current;
  daily =   new OW_daily;
  hourly =  new OW_hourly;


  Serial.print("Lat = "); Serial.print(latitude);
  Serial.print(", Lon = "); Serial.println(longitude);
  Serial.println("");
  Serial.println("location lat lon from littlefs config.txt");
  Serial.print("https://www.google.com/search?q="); Serial.print(latitude); Serial.print(",");Serial.println(longitude);
  Serial.println("");
  
  bool parsed = ow.getForecast(current, hourly, daily, api_key, latitude, longitude, units, language, true);

  if (parsed) Serial.println("Data points received");
  else Serial.println("Failed to get data points");

  //Serial.print("Free heap = "); Serial.println(ESP.getFreeHeap(), DEC);

  printWeather(); // For debug, turn on output with #define SERIAL_MESSAGES

  if (booted)
  {
    drawProgress(100, "Done...");
    delay(2000);
    tft.fillScreen(TFT_BLACK);
  }
  else
  {
    fillSegment(22, 22, 0, 360, 16, TFT_NAVY);
    fillSegment(22, 22, 0, 360, 22, TFT_BLACK);
  }

  if (parsed)
  {
    tft.loadFont(AA_FONT_SMALL, LittleFS);
    drawCurrentWeather();
    drawForecast();
    drawAstronomy();
    tft.unloadFont();

    // Update the temperature here so we don't need to keep
    // loading and unloading font which takes time
    tft.loadFont(AA_FONT_LARGE, LittleFS);
    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);

    // Font ASCII code 0xB0 is a degree symbol, but o used instead in small font
    tft.setTextPadding(tft.textWidth(" -88")); // Max width of values

    String weatherText = "";
    weatherText = String(current->temp, 0);  // Make it integer temperature
    tft.drawString(weatherText, 215, 95); //  + "°" symbol is big... use o in small font
    tft.unloadFont();
  }
  else
  {
    Serial.println("Failed to get weather");
  }

  // Delete to free up space
  delete current;
  delete hourly;
  delete daily;
}

/***************************************************************************************
**                          Update progress bar
***************************************************************************************/
void drawProgress(uint8_t percentage, String text) {
  tft.loadFont(AA_FONT_SMALL, LittleFS);
  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextPadding(240);
  tft.drawString(text, 120, 260);

  ui.drawProgressBar(10, 269, 240 - 20, 15, percentage, TFT_WHITE, TFT_BLUE);

  tft.setTextPadding(0);
  tft.unloadFont();
}

/***************************************************************************************
**                          Draw the clock digits
***************************************************************************************/
void drawTime() {
  tft.loadFont(AA_FONT_LARGE, LittleFS);

  // Convert UTC to local time, returns zone code in tz1_Code, e.g "GMT"
  time_t local_time = TIMEZONE.toLocal(now(), &tz1_Code);

  String timeNow = "";

  if (hour(local_time) < 10) timeNow += "0";
  timeNow += hour(local_time);
  timeNow += ":";
  if (minute(local_time) < 10) timeNow += "0";
  timeNow += minute(local_time);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" 44:44 "));  // String width + margin
  tft.drawString(timeNow, 120, 53);

  drawSeparator(51);

  tft.setTextPadding(0);

  tft.unloadFont();
}

/***************************************************************************************
**                          Draw the current weather
***************************************************************************************/
void drawCurrentWeather() {
  String date = "Updated: " + strDate(current->dt);
  String weatherText = "None";

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" Updated: Mmm 44 44:44 "));  // String width + margin
  tft.drawString(date, 120, 16);

  String weatherIcon = "";

  String currentSummary = current->main;
  currentSummary.toLowerCase();

  weatherIcon = getMeteoconIcon(current->id, true);

  //uint32_t dt = millis();
  //ui.drawBmp("/icon/" + weatherIcon + ".bmp", 0, 53);
  TJpgDec.drawFsJpg( 0, 53, "/icon/" + weatherIcon + ".jpg", LittleFS);


  //Serial.print("Icon draw time = "); Serial.println(millis()-dt);

  // Weather Text
  if (language == "en")
    weatherText = current->main;
  else
    weatherText = current->description;

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);

  int splitPoint = 0;
  int xpos = 235;
  splitPoint =  splitIndex(weatherText);

  tft.setTextPadding(xpos - 100);  // xpos - icon width
  if (splitPoint) tft.drawString(weatherText.substring(0, splitPoint), xpos, 69);
  else tft.drawString(" ", xpos, 69);
  tft.drawString(weatherText.substring(splitPoint), xpos, 86);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(TR_DATUM);
  tft.setTextPadding(0);
  if (units == "metric") tft.drawString("oC", 237, 95);
  else  tft.drawString("oF", 237, 95);

  //Temperature large digits added in updateData() to save swapping font here

  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  weatherText = String(current->wind_speed, 0);

  if (units == "metric") weatherText += " m/s";
  else weatherText += " mph";

  tft.setTextDatum(TC_DATUM);
  tft.setTextPadding(tft.textWidth("888 m/s")); // Max string length?
  tft.drawString(weatherText, 124, 136);

  if (units == "imperial")
  {
    weatherText = current->pressure;
    weatherText += " in";
  }
  else
  {
    weatherText = String(current->pressure, 0);
    weatherText += " hPa";
  }

  tft.setTextDatum(TR_DATUM);
  tft.setTextPadding(tft.textWidth(" 8888hPa")); // Max string length?
  tft.drawString(weatherText, 230, 136);



  int windAngle = (current->wind_deg + 22.5) / 45;
  if (windAngle > 7) windAngle = 0;
  String wind[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW" };
  tft.drawString(wind[windAngle], 150, 70); //luberth draw wind direction in text == arrow confuse me == going or comming
  //ui.drawBmp("/wind/" + wind[windAngle] + ".bmp", 101, 86);
  TJpgDec.drawFsJpg(  101, 86, "/wind/" + wind[windAngle] + ".jpg", LittleFS);
  
  drawSeparator(153);

  tft.setTextDatum(TL_DATUM); // Reset datum to normal
  tft.setTextPadding(0);      // Reset padding width to none
}

/***************************************************************************************
**                          Draw the 4 forecast columns
***************************************************************************************/
// draws the three forecast columns
void drawForecast() {
  int8_t dayIndex = 1;

  drawForecastDetail(  8, 171, dayIndex++);
  drawForecastDetail( 66, 171, dayIndex++); // was 95
  drawForecastDetail(124, 171, dayIndex++); // was 180
  drawForecastDetail(182, 171, dayIndex  ); // was 180
  drawSeparator(171 + 69);
}

/***************************************************************************************
**                          Draw 1 forecast column at x, y
***************************************************************************************/
// helper for the forecast columns
void drawForecastDetail(uint16_t x, uint16_t y, uint8_t dayIndex) {

  if (dayIndex >= MAX_DAYS) return;

  String day  = shortDOW[weekday(TIMEZONE.toLocal(daily->dt[dayIndex], &tz1_Code))];
  day.toUpperCase();

  tft.setTextDatum(BC_DATUM);

  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth("WWW"));
  tft.drawString(day, x + 25, y);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth("-88   -88"));
  String highTemp = String(daily->temp_max[dayIndex], 0);
  String lowTemp  = String(daily->temp_min[dayIndex], 0);
  tft.drawString(highTemp + " " + lowTemp, x + 25, y + 17);

  String weatherIcon = getMeteoconIcon(daily->id[dayIndex], false);

  //ui.drawBmp("/icon50/" + weatherIcon + ".bmp", x, y + 18);
  TJpgDec.drawFsJpg( x, y + 18, "/icon50/" + weatherIcon + ".jpg", LittleFS);
  
  tft.setTextPadding(0); // Reset padding width to none
}

/***************************************************************************************
**                          Draw Sun rise/set, Moon, cloud cover and humidity
***************************************************************************************/
void drawAstronomy() {

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" Last qtr "));

  time_t local_time = TIMEZONE.toLocal(current->dt, &tz1_Code);
  uint16_t y = year(local_time);
  uint8_t  m = month(local_time);
  uint8_t  d = day(local_time);
  uint8_t  h = hour(local_time);
  int      ip;
  uint8_t icon = moon_phase(y, m, d, h, &ip);

  tft.drawString(moonPhase[ip], 120, 319);
  //ui.drawBmp("/moon/moonphase_L" + String(icon) + ".bmp", 120 - 30, 318 - 16 - 60);
  TJpgDec.drawFsJpg( 120 - 30, 318 - 16 - 60, "/moon/moonphase_L" + String(icon) + ".jpg", LittleFS);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextPadding(0); // Reset padding width to none
  tft.drawString(sunStr, 40, 270);

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" 88:88 "));

  String rising = strTime(current->sunrise) + " ";
  int dt = rightOffset(rising, ":"); // Draw relative to colon to them aligned
  tft.drawString(rising, 40 + dt, 290);

  String setting = strTime(current->sunset) + " ";
  dt = rightOffset(setting, ":");
  tft.drawString(setting, 40 + dt, 305);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawString(cloudStr, 195, 260);

  String cloudCover = "";
  cloudCover += current->clouds;
  cloudCover += "%";

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth(" 100%"));
  tft.drawString(cloudCover, 210, 277);

  tft.setTextDatum(BC_DATUM);
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawString(humidityStr, 195, 300 - 2);

  String humidity = "";
  humidity += current->humidity;
  humidity += "%";

  tft.setTextDatum(BR_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextPadding(tft.textWidth("100%"));
  tft.drawString(humidity, 210, 315);

  tft.setTextPadding(0); // Reset padding width to none
}

/***************************************************************************************
**                          Get the icon file name from the index number
***************************************************************************************/
const char* getMeteoconIcon(uint16_t id, bool today)
{
  if ( today && id / 100 == 8 && (current->dt < current->sunrise || current->dt > current->sunset)) id += 1000;

  if (id / 100 == 2) return "thunderstorm";
  if (id / 100 == 3) return "drizzle";
  if (id / 100 == 4) return "unknown";
  if (id == 500) return "lightRain";
  else if (id == 511) return "sleet";
  else if (id / 100 == 5) return "rain";
  if (id >= 611 && id <= 616) return "sleet";
  else if (id / 100 == 6) return "snow";
  if (id / 100 == 7) return "fog";
  if (id == 800) return "clear-day";
  if (id == 801) return "partly-cloudy-day";
  if (id == 802) return "cloudy";
  if (id == 803) return "cloudy";
  if (id == 804) return "cloudy";
  if (id == 1800) return "clear-night";
  if (id == 1801) return "partly-cloudy-night";
  if (id == 1802) return "cloudy";
  if (id == 1803) return "cloudy";
  if (id == 1804) return "cloudy";

  return "unknown";
}

/***************************************************************************************
**                          Draw screen section separator line
***************************************************************************************/
// if you don't want separators, comment out the tft-line
void drawSeparator(uint16_t y) {
  tft.drawFastHLine(10, y, 240 - 2 * 10, 0x4228);
}

/***************************************************************************************
**                          Determine place to split a line line
***************************************************************************************/
// determine the "space" split point in a long string
int splitIndex(String text)
{
  uint16_t index = 0;
  while ( (text.indexOf(' ', index) >= 0) && ( index <= text.length() / 2 ) ) {
    index = text.indexOf(' ', index) + 1;
  }
  if (index) index--;
  return index;
}

/***************************************************************************************
**                          Right side offset to a character
***************************************************************************************/
// Calculate coord delta from end of text String to start of sub String contained within that text
// Can be used to vertically right align text so for example a colon ":" in the time value is always
// plotted at same point on the screen irrespective of different proportional character widths,
// could also be used to align decimal points for neat formatting
int rightOffset(String text, String sub)
{
  int index = text.indexOf(sub);
  return tft.textWidth(text.substring(index));
}

/***************************************************************************************
**                          Left side offset to a character
***************************************************************************************/
// Calculate coord delta from start of text String to start of sub String contained within that text
// Can be used to vertically left align text so for example a colon ":" in the time value is always
// plotted at same point on the screen irrespective of different proportional character widths,
// could also be used to align decimal points for neat formatting
int leftOffset(String text, String sub)
{
  int index = text.indexOf(sub);
  return tft.textWidth(text.substring(0, index));
}

/***************************************************************************************
**                          Draw circle segment
***************************************************************************************/
// Draw a segment of a circle, centred on x,y with defined start_angle and subtended sub_angle
// Angles are defined in a clockwise direction with 0 at top
// Segment has radius r and it is plotted in defined colour
// Can be used for pie charts etc, in this sketch it is used for wind direction
#define DEG2RAD 0.0174532925 // Degrees to Radians conversion factor
#define INC 2 // Minimum segment subtended angle and plotting angle increment (in degrees)
void fillSegment(int x, int y, int start_angle, int sub_angle, int r, unsigned int colour)
{
  // Calculate first pair of coordinates for segment start
  float sx = cos((start_angle - 90) * DEG2RAD);
  float sy = sin((start_angle - 90) * DEG2RAD);
  uint16_t x1 = sx * r + x;
  uint16_t y1 = sy * r + y;

  // Draw colour blocks every INC degrees
  for (int i = start_angle; i < start_angle + sub_angle; i += INC) {

    // Calculate pair of coordinates for segment end
    int x2 = cos((i + 1 - 90) * DEG2RAD) * r + x;
    int y2 = sin((i + 1 - 90) * DEG2RAD) * r + y;

    tft.fillTriangle(x1, y1, x2, y2, x, y, colour);

    // Copy segment end to segment start for next segment
    x1 = x2;
    y1 = y2;
  }
}

/***************************************************************************************
**                          Print the weather info to the Serial Monitor
***************************************************************************************/
void printWeather(void)
{
#ifdef SERIAL_MESSAGES
  Serial.println("Weather from OpenWeather\n");

  Serial.println("############### Current weather ###############\n");
  Serial.print("dt (time)          : "); Serial.println(strDate(current->dt));
  Serial.print("sunrise            : "); Serial.println(strDate(current->sunrise));
  Serial.print("sunset             : "); Serial.println(strDate(current->sunset));
  Serial.print("main               : "); Serial.println(current->main);
  Serial.print("temp               : "); Serial.println(current->temp);
  Serial.print("humidity           : "); Serial.println(current->humidity);
  Serial.print("pressure           : "); Serial.println(current->pressure);
  Serial.print("wind_speed         : "); Serial.println(current->wind_speed);
  Serial.print("wind_deg           : "); Serial.println(current->wind_deg);
  Serial.print("clouds             : "); Serial.println(current->clouds);
  Serial.print("id                 : "); Serial.println(current->id);
  Serial.println();

  Serial.println("###############  Daily weather  ###############\n");
  Serial.println();

  for (int i = 0; i < 5; i++)
  {
    Serial.print("dt (time)          : "); Serial.println(strDate(daily->dt[i]));
    Serial.print("id                 : "); Serial.println(daily->id[i]);
    Serial.print("temp_max           : "); Serial.println(daily->temp_max[i]);
    Serial.print("temp_min           : "); Serial.println(daily->temp_min[i]);
    Serial.println();
  }

#endif
}
/***************************************************************************************
**             Convert Unix time to a "local time" time string "12:34"
***************************************************************************************/
String strTime(time_t unixTime)
{
  time_t local_time = TIMEZONE.toLocal(unixTime, &tz1_Code);

  String localTime = "";

  if (hour(local_time) < 10) localTime += "0";
  localTime += hour(local_time);
  localTime += ":";
  if (minute(local_time) < 10) localTime += "0";
  localTime += minute(local_time);

  return localTime;
}

/***************************************************************************************
**  Convert Unix time to a local date + time string "Oct 16 17:18", ends with newline
***************************************************************************************/
String strDate(time_t unixTime)
{
  time_t local_time = TIMEZONE.toLocal(unixTime, &tz1_Code);

  String localDate = "";

  localDate += monthShortStr(month(local_time));
  localDate += " ";
  localDate += day(local_time);
  localDate += " " + strTime(unixTime);

  return localDate;
}

/**The MIT License (MIT)
  Copyright (c) 2015 by Daniel Eichhorn
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYBR_DATUM HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  See more at http://blog.squix.ch
*/

//  Changes made by Bodmer:

//  Minor changes to text placement and auto-blanking out old text with background colour padding
//  Moon phase text added (not provided by OpenWeather)
//  Forecast text lines are automatically split onto two lines at a central space (some are long!)
//  Time is printed with colons aligned to tidy display
//  Min and max forecast temperatures spaced out
//  New smart splash startup screen and updated progress messages
//  Display does not need to be blanked between updates
//  Icons nudged about slightly to add wind direction + speed
//  Barometric pressure added

//  Adapted to use the OpenWeather library: https://github.com/Bodmer/OpenWeather
//  Moon phase/rise/set (not provided by OpenWeather) replace with  and cloud cover humidity
//  Created and added new 100x100 and 50x50 pixel weather icons, these are in the
//  sketch data folder, press Ctrl+K to view
//  Add moon icons, eliminate all downloads of icons (may lose server!)
//  Adapted to use anti-aliased fonts, tweaked coords
//  Added forecast for 4th day
//  Added cloud cover and humidity in lieu of Moon rise/set
//  Adapted to be compatible with ESP32


//*************************************************************************
//
// https://github.com/ldijkman/OpenWeather
// https://github.com/ldijkman/randomnerd_esp32_wifi_manager/tree/main/ESP8266-TFT_eSPI
// https://ldijkman.github.io/randomnerd_esp32_wifi_manager/Touch_Electra-Electra_Touch/
//
//  4 electa read some settings from /data littlefs /config.txt
// so that you can change it in future from ace js inbrowser cloud web editor
// so that it can/will become part of electra touch tft browser install ESP8266 4mbyte ESP12E ESP12F
// luberth
//************************************************************************
