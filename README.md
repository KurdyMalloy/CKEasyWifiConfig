# CKEasyWifiConfig #
A library for the Digilent chipKIT development boards. It lets you configure the wifi settings of the digilent wifi shield from a configuration file on the SD card.

##Why##
###Problem###
Most sketches that you find in the examples always hard-code the wifi settings in the sketch; this is kind of problematic if every time that you want to connect to a different network you have to change code and rebuild the sketch and reload it on the board. 
###Solution###
This library will let you configure the wifi shield with settings read from a configuration file saved on a SD card loaded on the shield. So to change the settings, you just have to change the configuration file on the SD card. This is a lot better especially when your device is deployed in a project.

## Installation ##
This is arduino's standard way of installing a custom library:

- Copy the folder named "CKEasyWifiConfig" from the repository to the sub-folder 
 **/libraries/** under your sketchbook directory.
- Use the "NetConf.cfg" file from the root of the repository as a template for your configuration file that will go on the SD card.

## Dependencies ##
The library is dependent on four other libraries:

- Streaming  [http://arduiniana.org/libraries/streaming/](http://arduiniana.org/libraries/streaming/ "Streaming5")
- SD (Already part of mpide)
- DNETcK  [http://www.digilentinc.com/Products/Detail.cfm?NavPath=2,892,1037&Prod=CHIPKIT-WIFI-SHIELD/](http://www.digilentinc.com/Products/Detail.cfm?NavPath=2,892,1037&Prod=CHIPKIT-WIFI-SHIELD/ "DNETcK")
- DWIFIcK [http://www.digilentinc.com/Products/Detail.cfm?NavPath=2,892,1037&Prod=CHIPKIT-WIFI-SHIELD/](http://www.digilentinc.com/Products/Detail.cfm?NavPath=2,892,1037&Prod=CHIPKIT-WIFI-SHIELD/ "DWIFIcK")

You need to include these in your main sketch before you include this one.

This library was tested with mpide 0023.

## Configuration File ##
The configuration file included in the project is already self-documented.

Any line beginning with the character '#' or '/' will be ignored and considered a comment.

The parameter names in the file are case sensitive, they need to be uppercase. Here is the possible values for each parameters:

**SECURITY:**

- OPEN
- WPA2_WITH_KEY
- WPA2_WITH_PASS_PHRASE
- WPA_WITH_KEY
- WPA_WITH_PASS_PHRASE
- WPA_AUTO_WITH_KEY
- WPA_AUTO_WITH_PASS_PHRASE
- WEP_40
- WEP_104

**IP:**

- A valid ipv4 address in the form xxx.xxx.xxx.xxx
- DHCP or left blank to use dhcp

**SSID:**

- The ssid of your network; it is usually case sensitive.

**KEY:**

- Your key in hexadecimal if you can provide it.

**KEYCHAR:**

- Your key if you want to provide it in text.

**KEYINDEX:**

- The index of your key if it is indexed


Important: the KEY and KEYCHAR parameters are mutually exclusive; if you provide both, the last one read in the file will be the one that is used.

Example of a config file to connect with dhcp and an hexadecimal wpa2 key :

    SECURITY:WPA_AUTO_WITH_KEY
	SSID:MYNETWORK
	IP:DHCP
	KEY:5312826e170e6d0ef8691670cabc5c9c3072cf8c003e4ee541a499d060746cfa

Example of a config file to connect with a static address and a wpa2 passphrase key :

    SECURITY:WPA_AUTO_WITH_PASS_PHRASE
	SSID:MYNETWORK
	IP:192.168.1.252
	KEYCHAR:hermionegranger

## Usage ##
There is two example projects provided with the library; one to use a configuration file on the card and one where you can pass the security infos in the code.

To use the library, make sure that you include all the dependency libraries before you include CKEasyWifiConfig.

If you want the library to output some debugging information in the serial port, make sure to define "CKEASYWIFICONFIG_SERIAL_DEBUG" before to include the library. Also you will have to make sure that you setup the serial port before you call the "Setup()" method of the library.

It is also very important to initialize the SD card before you call the "Setup()" method of the CKEasyWifiConfig class since it will use it.

Example:

	#include <SD.h>
	#include <Streaming.h>
	#include <WiFiShieldOrPmodWiFi.h>  // This is for the MRF24WBxx on a pmodWiFi or WiFiShield
	//#include <WiFiShieldOrPmodWiFi_G.h> // This is for the MRF24WGxx on a pmodWiFi or WiFiShield
	#include <DNETcK.h>
	#include <DWIFIcK.h>

	// Remove the next line to stop the serial debugging of the library
	#define CKEASYWIFICONFIG_SERIAL_DEBUG  
	#include <CKEasyWifiConfig.h>

	//To enable serial debugging in the main sketch
	#define USE_SERIAL_DEBUG  

	// Name of the config file on the SD card
	char networkConfigFile[] = "NetConf.cfg";

	CKEasyWifiConfig networkConfig(networkConfigFile);

	void setup() {
	  // put your setup code here, to run once:

	  #ifdef USE_SERIAL_DEBUG
		Serial.begin(9600);
		Serial << "Initializing SD card..." << endl;
	  #endif

	  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
	  if (!SD.begin(4)) {
		#ifdef USE_SERIAL_DEBUG
		  Serial << "SD Card initialization failed!" << endl;
		#endif
		return;
	  }
	  else
	  {
		#ifdef USE_SERIAL_DEBUG
		  Serial << "SD Card initialization success." << endl;
		  if (SD.exists(networkConfigFile))
			Serial << "Network Config File --> " << networkConfigFile << " is present on the SD card." << endl;
		  else
			Serial << "Network Config File --> " << networkConfigFile << " is NOT present on the SD card." << endl;
		#endif // USE_SERIAL_DEBUG
	  }
	  
	  // Call the initialization
	  networkConfig.Setup();
	}

	void loop() {
	  // put your main code here, to run repeatedly: 

	  // every pass through loop(), keep the IP stack alive
	  DNETcK::periodicTasks(); 
	}

## Limitations ##
The SD card library only supports file names in the 8.3 format. So your configuration file and any sub-folder that you want to put it in, need to have a name in this format.

Due to some architecture differences; this library is not really suitable for the arduino platform. The arduino has a very different memory model especially to store static stuff into the flash memory (PROGMEM). This code was not designed with that in mind and take fully advantage of the fact that the PIC architecture is much easier to code for. Also since there is a lot more space available on the chipKIT platform, more time was spent developing for the usability of the library than on trying to reduce its size. The dependency to the Digilent networking libraries is also a factor to the non-portability of the library.

## Praise##
I know that there was a lot of critics in the forums in regards to the compatibility of the chipKIT environment with the Arduino. But looking at some of the files under the hood of mpide I realized the amount of work necessary to bring this to life. I really think that this is quite an achievement.

The chipKIT platform is amazing; I love the speed, the memory space and the color ;-)



 