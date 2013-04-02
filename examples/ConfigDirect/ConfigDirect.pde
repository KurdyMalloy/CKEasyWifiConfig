#include <SD.h>
#include <Streaming.h>
#include <WiFiShieldOrPmodWiFi.h>                       // This is for the MRF24WBxx on a pmodWiFi or WiFiShield
//#include <WiFiShieldOrPmodWiFi_G.h>                     // This is for the MRF24WGxx on a pmodWiFi or WiFiShield
#include <DNETcK.h>
#include <DWIFIcK.h>

// Remove the next line to stop the serial debugging of the library
#define CKEASYWIFINETWORK_SERIAL_DEBUG  
#define CKEASYWIFINETWORK_SUPPRESS_SD  // This line force the library to remove the SD card functionality; to save precious memory 
#include <CKEasyWifiNetwork.h>

//To enable serial debugging in the main sketch
#define USE_SERIAL_DEBUG  

//Security infos ; the structure is documented in the digilent DWIFIcK library documentation
//This one uses a key
/*
DWIFIcK::SECINFO secinfo = {
  DWIFIcK::WF_SECURITY_WPA2_WITH_KEY,
  {
    0,
    0,
    DWIFIcK::WPA2KEY{ 0x53, 0x12, 0x82, 0x6B, 0x16, 0x0E, 0x6E, 0x0E, 0xF8, 0x69, 0x16, 0x70, 0xCA, 0xBC, 0x5C, 0x9C, 0x30, 0x72, 0xCF, 0x8B, 0x00, 0x3E, 0x4E, 0xE5, 0x41, 0xA4, 0x99, 0xD0, 0x60, 0x74, 0x6C, 0xF9 }
  }
};
*/

// This one uses a passphrase ; it takes longer to connect as the key has to be computed in software
DWIFIcK::SECINFO secinfo = {
  DWIFIcK::WF_SECURITY_WPA2_WITH_PASS_PHRASE,
  {
    0,
    0,
    { "hermionegranger" }
  }
};

// Set the Library to connect by DHCP to the network with ssid "MYNETWORK"
CKEasyWifiNetwork networkWrapper(&secinfo, "MYNETWORK");

// Set the Library to connect with a static adress to the network with ssid "MYNETWORK"
//CKEasyWifiNetwork networkWrapper(&secinfo, "MYNETWORK", IPv4{192,168,1,250});

void setup() {
  // put your setup code here, to run once:

  #ifdef USE_SERIAL_DEBUG
    Serial.begin(9600);
  #endif

  // Call the initialization
  networkWrapper.Setup();
}

void loop() {
  // put your main code here, to run repeatedly: 

  // every pass through loop(), keep the IP stack alive
  networkWrapper.Loop(); 
}

