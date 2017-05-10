# Terrarium Light
##### An ESP8266 controlled Neopixel ring, imitating the Sun
-------------------------------------------------------------

<p align="center"> <a href="http://wolftech.me/wp-content/uploads/2017/05/Terrarium.jpg"><img src="https://i1.wp.com/wolftech.me/wp-content/uploads/2017/05/Terrarium.jpg?fit=300%2C225"></a> </p>

#### What is it?
* Arduino code for using an ESP8266 to light up a few WS2812 LEDs
* Fetches time from NTP servers and adjusts light according to time of day
* Accepts HTTP GET requests to change the default color
* Uses the code from my [Bare_minimum_WiFi_with_OTA](https://github.com/PRO2XY/Bare_minimum_WiFi_with_OTA) repo for seamless WiFi and OTA integration
* A lot of fun!

#### How to use it?
* Clone (or download) the repo into your Arduino sketchbook directory.
* Install the prerequisites (see below)
* Adjust the default parameters to your liking
* Upload to your ESP8266 using any of the upload methods
* Look at it come alive!

#### What should you be careful about?
* This is just a few hours of late night work, some bugs may be present
* I have not bothered with comments unless there's something not very clear
* Timezone needs to be set in 1 hour resolution at line 68 (```configTime(...)``` )
* If your timezone is not a full hour difference from GMT, add the remaining seconds on line 83. ```currentTime += 1800;``` (1800 gives an extra half hour)
* To set a color via HTTP, enter the request as ```IPADDRESS/setColor?c=AABBCC``` where AABBCC is the HEX code of the color.


#### Things to do
* This is a work in progress but not a very critical project. I will tweak things if I want to add or change something. 
* Pull requests are welcome

#### Prerequisites
* [Arduino IDE 1.6.6 or above](https://www.arduino.cc/en/main/software)
* [ESP8266 Core for Arduino IDE](https://github.com/esp8266/Arduino)
* [FastLED library](https://github.com/FastLED/)
