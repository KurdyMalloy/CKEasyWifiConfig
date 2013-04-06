#include <SD.h>
#include <Streaming.h>
#include <WiFiShieldOrPmodWiFi.h>                       // This is for the MRF24WBxx on a pmodWiFi or WiFiShield
//#include <WiFiShieldOrPmodWiFi_G.h>                     // This is for the MRF24WGxx on a pmodWiFi or WiFiShield
#include <DNETcK.h>
#include <DWIFIcK.h>

// Remove the next line to stop the serial debugging of the library
#define CKEASYWIFICONFIG_SERIAL_DEBUG  
#include <CKEasyWifiConfig.h>

//To enable serial debugging in the main sketch
#define USE_SERIAL_DEBUG  

// Name of the config file on the SD card
char networkConfigFile[] = "NetConf.cfg";

CKEasyWifiConfig networkWrapper(networkConfigFile);

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
  networkWrapper.Setup();
}

void loop() {
  // put your main code here, to run repeatedly: 

  // every pass through loop(), keep the IP stack alive
  networkWrapper.Loop(); 
}
