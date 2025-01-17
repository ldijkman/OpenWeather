//  Use the OpenWeather library: https://github.com/Bodmer/OpenWeather

//  The weather icons and fonts are in the sketch data folder, press Ctrl+K
//  to view.

// The ESP32 board support package 2.0.0 or later must be loaded in the
// Arduino boards manager to provide LittleFS support.

//            >>>       IMPORTANT TO PREVENT CRASHES      <<<
//>>>>>>  Set LittleFS to at least 1.5Mbytes before uploading files  <<<<<<

//            >>>           DON'T FORGET THIS             <<<
//  Upload the fonts and icons to LittleFS using the "Tools" menu option.

// You can change the number of hours and days for the forecast in the
// "User_Setup.h" file inside the OpenWeather library folder.
// By default this is 6 hours (can be up to 48) and 5 days
// (can be up to 8 days = today plus 7 days). This sketch requires
// at least 5 days of forecast. Forecast hours can be set to 1 as
// the hourly forecast data is not used in this sketch.

//////////////////////////////
// Setttings defined below


// next wifi will be done by electra in future
#define WIFI_SSID      "KaRo"
#define WIFI_PASSWORD  "KaRo"

#define TIMEZONE euCET // See NTP_Time.h tab for other "Zone references", UK, usMT etc

// Update every 15 minutes, up to 1000 request per day are free (viz average of ~40 per hour)
const int UPDATE_INTERVAL_SECS = 2UL * 60UL; // 15 minutes

// Pins for the TFT interface are defined in the User_Config.h file inside the TFT_eSPI library

// For units use "metric" or "imperial"
const String units = "metric";



  // some config moved to litlefs /config.txt file in  /data/config.txt




// For language codes see https://openweathermap.org/current#multi
const String language = "en"; // Default language = en = English

// Short day of week abbreviations used in 4 day forecast (change to your language)
const String shortDOW [8] = {"???", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

// Change the labels to your language here:
const char sunStr[]        = "Sun";
const char cloudStr[]      = "Cloud";
const char humidityStr[]   = "Humidity";
const String moonPhase [8] = {"New", "Waxing", "1st qtr", "Waxing", "Full", "Waning", "Last qtr", "Waning"};

// End of user settings
//////////////////////////////
