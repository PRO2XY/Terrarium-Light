#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

// For WiFiManager
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <FS.h> // For LED webserver

// For ArduinoOTA
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
uint8_t OTAProgressIndicator;

// For Time
#include <time.h>
time_t currentTime;

// For heartbeat LED
#include <Ticker.h>
Ticker heartbeat_Tick;
#define LEDPIN  2   // LED pin to show heartbeat on; 2 is internal on ESP-12
Ticker WS2812_Tick;

#define SLEEP_TIME  10*60*1000 // milliseconds

// For LEDs
#include <FastLED.h>
#define DATA_PIN 14
#define NUM_LEDS 12
#define BRIGHTNESS 190

CRGB leds[NUM_LEDS];
CRGB defaultColor = 0xFFCCBB;


ESP8266WebServer server(80);


void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  pinMode(LEDPIN, OUTPUT);
  heartbeat_Tick.attach(0.5, heartbeat);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  WS2812_Tick.attach(0.1, setup_animation);

  SPIFFS.begin();
  setup_wifi(); // in setup_functions tab
  setup_OTA();
  setup_server();


  // Setup done. Turn off heartbeat LED (change this to suit your application)
  heartbeat_Tick.detach();
  digitalWrite(LEDPIN, HIGH);

  WS2812_Tick.detach();
  fadeToBlackIn(1000); // milliseconds
  delay(200);

  for (CRGB & pixel : leds) {pixel = defaultColor;}
  fadeUpToBrightnessIn(BRIGHTNESS , 5000);

  configTime(3600 * 5, 0, "time.nist.gov", "www.pool.ntp.org/zone/asia"); // GMT +5.5 Hrs
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
  
  static bool timeSet = false;
  time(&currentTime);
  
  if (!timeSet && currentTime != 0) {
    // Time is set!
    timeSet = true;
  }
  if (timeSet){
    currentTime += 1800; // Workaround to add half an hour to timezone
    adjustLEDs();
  }
}


// Fade all the way to black in _ms milliseconds
// This is a BLOCKING function
// Can be made unblocked by using Ticker (but I don't need it here)

void fadeToBlackIn(uint16_t _ms) {
  while ( FastLED.getBrightness() > 0) {
    //fadeToBlackBy(leds, NUM_LEDS, 1);
    FastLED.setBrightness(FastLED.getBrightness() - 1);
    FastLED.show();
    yield();                // ** Workaround **
    ArduinoOTA.handle();    // to allow critical
    server.handleClient();  // functions to work
    delay(round(_ms / 255));
  }
}


// To Do: Make this function non-blocking (long delays 60 seconds are possible during boot)
void fadeUpToBrightnessIn(uint8_t _brightness, uint16_t _ms) {
  while (FastLED.getBrightness() < _brightness){
    FastLED.setBrightness(FastLED.getBrightness() + 1);
    FastLED.show();
    yield();                // ** Workaround **
    ArduinoOTA.handle();    // to allow critical
    server.handleClient();  // functions to work
    delay(round(_ms / 255));
  }
}

void adjustLEDs() {
  static uint8_t prevMinute = 0;
  static bool firstCall = true;

  struct tm *tm_struct = localtime(&currentTime);
  uint8_t hourOfDay = tm_struct->tm_hour;
  uint8_t minute = tm_struct->tm_min;

  // Workaround for time being set when minute is 0
  if (firstCall && minute == 0) {
    prevMinute = 59; // Set prevMinute to a different value
    // So the next if is true.
    firstCall = false;
  }

  if (prevMinute != minute) {
    // Time has changed
    prevMinute = minute;
    Serial.println(ctime(&currentTime));
    Serial.print(hourOfDay);
    Serial.print(":");
    Serial.println(minute);

    if (hourOfDay < 5) {
      // If LEDs are on, fade them off
      if (FastLED.getBrightness() > 0) fadeToBlackIn(10000);
    }
    else if (hourOfDay < 6) {
      fadeUpToBrightnessIn(63, 60000);
    }
    else if (hourOfDay < 7) {
      fadeUpToBrightnessIn(63 + (minute / 59.0) * 64, 1000);
    }
    else if (hourOfDay < 8) {
      fadeUpToBrightnessIn(127 + (minute / 59.0) * 64, 1000);
    }
    else if (hourOfDay < 9) {
      fadeUpToBrightnessIn(191 + (minute / 59.0) * 64, 1000); // Full bright by 9 AM
    }
    else if (hourOfDay < 17) {
      fadeUpToBrightnessIn(255, 1000);
    } // Nothing to do till 4 PM
    else if (hourOfDay < 18) {
      fadeUpToBrightnessIn(191 - (minute / 59.0) * 64, 1000);
    }
    else if (hourOfDay < 19) {
      fadeUpToBrightnessIn(127 - (minute / 59.0) * 64, 1000);
    }
    else if (hourOfDay < 20) {
      fadeUpToBrightnessIn(63 - (minute / 59.0) * 63, 1000);
    }
    else {
      fadeToBlackIn(10000);
    }
    Serial.print("Brightness: ");
    Serial.println(FastLED.getBrightness());
    FastLED.show();
  }
}

