/*
   CKEasyWifiConfig, a wrapper around the Digilent WIFI library for the ChipKit platform
   Copyright 2013 Jean-Michel Julien

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#ifndef CKEASYWIFICONFIG_H_
#define CKEASYWIFICONFIG_H_

// UnComment this line to activate serial debugging, also make sure that the Serial interface is initialized
//#define CKEASYWIFICONFIG_SERIAL_DEBUG

// UnComment this line to suppress SD Card usage (Also add the SD library to your sketch and configure the SD library before making calls to this library)
//#define CKEASYWIFICONFIG_SUPPRESS_SD
// UnComment this line to suppress WF_Config.x file usage (See the DWIFIcK library documentation)
#define CKEASYWIFICONFIG_SUPPRESS_WF_CONFIG
// UnComment this line to suppress provided SECINFO structure usage (Connection infos contained in code)
//#define CKEASYWIFICONFIG_SUPPRESS_SECINFO

#include <DNETcK.h>
#include <DWIFIcK.h>
#ifndef CKEASYWIFICONFIG_SUPPRESS_SD
#include <SD.h>
#endif
#include <Streaming.h>

class CKEasyWifiConfig
{
public:
#ifndef CKEASYWIFICONFIG_SUPPRESS_WF_CONFIG
  //Constructor to use DHCP with WF_Config.x file for security parameters
  CKEasyWifiConfig();
  //Constructor to use StaticIP with WF_Config.x file for security parameters
  CKEasyWifiConfig(IPv4 ip);
#endif
#ifndef CKEASYWIFICONFIG_SUPPRESS_SECINFO
  //Constructor to use DHCP
  CKEasyWifiConfig(DWIFIcK::SECINFO* secinfo, const char* szSsid);
  //Constructor to use StaticIP
  CKEasyWifiConfig(DWIFIcK::SECINFO* secinfo, const char* szSsid, IPv4 ip);
#endif
#ifndef CKEASYWIFICONFIG_SUPPRESS_SD
  //Constructor that use the configuration parameters from a file on the SD Card
  CKEasyWifiConfig(char* szConfigFile);
#endif

  // To be called once at the beginning; usually in the setup method of the sketch
  bool Setup(); 

private:
  // Types of configuration methods
  typedef enum
  {
    WF_FILE = 0,
    SEC_INFO = 1,
    SD_FILE = 2
  }  Config_Type;

  const int connectionTimeout;  // tcp/ip connection timeout
  bool useDhcp;  // Do we use DHCP to connect
  IPv4 staticIp;  //Specify a StaticIP
  const char* ssid;  // Specify the SSID
  Config_Type typeConfig; //configuration type we are using
  char* configFile;  // Name of the config file on the SD card
#if !defined(CKEASYWIFICONFIG_SUPPRESS_SECINFO) || !defined(CKEASYWIFICONFIG_SUPPRESS_SD)
  DWIFIcK::SECINFO secInfo;  //Security Infos
#endif
#ifndef CKEASYWIFICONFIG_SUPPRESS_SD
  char _ssid[33];  //ssid read from SD card max 32 char 
#endif

  // method to read the configuration data from a file on the SD card
  void ReadConfigFile();
  // method to read a line of text from a file
  bool ReadLine(char *buffer, File &file);
};



#ifndef CKEASYWIFICONFIG_SUPPRESS_WF_CONFIG
//Constructor to use DHCP with WF_Config.x file for security parameters
CKEasyWifiConfig::CKEasyWifiConfig() : typeConfig(WF_FILE), useDhcp(true), connectionTimeout(40000) {}
//Constructor to use StaticIP with WF_Config.x file for security parameters
CKEasyWifiConfig::CKEasyWifiConfig(IPv4 ip) : typeConfig(WF_FILE), staticIp(ip), useDhcp(false), connectionTimeout(40000) {}
#endif //CKEASYWIFICONFIG_SUPPRESS_WF_CONFIG

#ifndef CKEASYWIFICONFIG_SUPPRESS_SECINFO
//Constructor to use DHCP
CKEasyWifiConfig::CKEasyWifiConfig(DWIFIcK::SECINFO* secinfo, const char* szSsid) : typeConfig(SEC_INFO),secInfo(*secinfo), ssid(szSsid), useDhcp(true), connectionTimeout(40000) {}
//Constructor to use StaticIP
CKEasyWifiConfig::CKEasyWifiConfig(DWIFIcK::SECINFO* secinfo, const char* szSsid, IPv4 ip) : typeConfig(SEC_INFO), secInfo(*secinfo), ssid(szSsid), staticIp(ip), useDhcp(false), connectionTimeout(40000) {}
#endif //CKEASYWIFICONFIG_SUPPRESS_SECINFO

#ifndef CKEASYWIFICONFIG_SUPPRESS_SD
//Constructor that use the configuration parameters from a file on the SD Card
CKEasyWifiConfig::CKEasyWifiConfig(char* szConfigFile) : typeConfig(SD_FILE), configFile(szConfigFile), connectionTimeout(40000) {}

// Max Line size for the read buffer 
const int ReadBufferLength = 100;

// Method to read a line from SD file
bool CKEasyWifiConfig::ReadLine(char *buffer, File &file)
{
  bool stop = false;
  char ch;
  int i = 0;
  
  //Cleanup buffer
  memset(buffer, '\0', ReadBufferLength);
  // Read the file into the read buffer until the next line
  do
  {
    if (!file.available())
    {
      #ifdef CKEASYWIFICONFIG_SERIAL_DEBUG
        Serial << "EASYWIFI: Error Reading line from config file; End of File" << endl;
      #endif
      return false;
    }
    ch = file.read();
    
    if (ch != '\r' && ch != '\n')
      buffer[i++] = ch;
    else
    {
      file.read(); //pass to next char; \r\n always come together
      stop = true; //end of line
    }
    if (i >= ReadBufferLength)
    {
      #ifdef CKEASYWIFICONFIG_SERIAL_DEBUG
        Serial << "EASYWIFI: Error Reading line from config file; line too long" << endl;
      #endif
      return false;  //Line too long to be read
    }
    if (!file.available())
    {
      #ifdef CKEASYWIFICONFIG_SERIAL_DEBUG
        Serial << "EASYWIFI: No end of line found; End of File" << endl;
      #endif
      stop = true;
    }
  } while (!stop);

  #ifdef CKEASYWIFICONFIG_SERIAL_DEBUG
    Serial << "EASYWIFI: Read line from config file --> " << buffer << endl;
  #endif
  return true;
}

// Method that read and interpret the config file
void CKEasyWifiConfig::ReadConfigFile()
{
  char readBuffer[ReadBufferLength];
  File fh = SD.open(configFile, FILE_READ);
  
  if(!fh)
  {
    #ifdef CKEASYWIFICONFIG_SERIAL_DEBUG
      Serial << "EASYWIFI: Cannot open SD config file --> " << configFile << endl;
    #endif
    return;
  }
  
  //read the file
  int completed = 0;
  while (ReadLine(readBuffer, fh))
  {
    if (readBuffer[0] == '#' || readBuffer[0] == '/')
      continue;  //  Commented line no need to parse
    char* tmp = strchr(readBuffer, ':');
    if (!tmp)
      continue; // Not a valid line
    *tmp = NULL;

    //Convert param name to uppercase
    for (int i = 0; i < strlen(readBuffer); i++)
      readBuffer[i] = toupper(readBuffer[i]);

    #ifdef CKEASYWIFICONFIG_SERIAL_DEBUG
      Serial << "EASYWIFI: Read network config Param --> " << readBuffer << " Value --> " << tmp+1 << endl;
    #endif
    
    if (!strcmp("SECURITY", readBuffer)) {

      completed++;
      for (int i = 0; i < strlen(tmp+1); i++)
        int i2 = i; //(tmp+1)[i] = toupper((tmp+1)[i]);
      if (!strcmp("OPEN", tmp+1)) {
        secInfo.securityType = DWIFIcK::WF_SECURITY_OPEN;
      } else if (!strcmp("WPA2_WITH_KEY", tmp+1)) {
        secInfo.securityType = DWIFIcK::WF_SECURITY_WPA2_WITH_KEY;
      } else if (!strcmp("WPA2_WITH_PASS_PHRASE", tmp+1)) {
        secInfo.securityType = DWIFIcK::WF_SECURITY_WPA2_WITH_PASS_PHRASE;
      } else if (!strcmp("WPA_WITH_KEY", tmp+1)) {
        secInfo.securityType = DWIFIcK::WF_SECURITY_WPA_WITH_KEY;
      } else if (!strcmp("WPA_WITH_PASS_PHRASE", tmp+1)) {
        secInfo.securityType = DWIFIcK::WF_SECURITY_WPA_WITH_PASS_PHRASE;
      } else if (!strcmp("WPA_AUTO_WITH_KEY", tmp+1)) {
        secInfo.securityType = DWIFIcK::WF_SECURITY_WPA_AUTO_WITH_KEY;
      } else if (!strcmp("WPA_AUTO_WITH_PASS_PHRASE", tmp+1)) {
        secInfo.securityType = DWIFIcK::WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE;
      } else if (!strcmp("WEP_40", tmp+1)) {
        secInfo.securityType = DWIFIcK::WF_SECURITY_WEP_40;
      } else if (!strcmp("WEP_104", tmp+1)) {
        secInfo.securityType = DWIFIcK::WF_SECURITY_WEP_104;
      } 

    } else if (!strcmp("IP", readBuffer)) {
      completed++;
      if (*(tmp+1) == '\0' || *(tmp+1) == 'D')
        useDhcp = true;
      else
      {
        useDhcp = false;
        char* next = strtok(tmp+1, ".");
        for (int i = 0; i < 4; i++)
        {
          staticIp.rgbIP[i] = strtoul(next, NULL, 10);
          next = strtok(NULL, ".");
        }
      }
    }else if (!strcmp("SSID", readBuffer)) {
      completed++;
      strcpy(_ssid, (tmp+1));
      ssid = _ssid;
    }else if (!strcmp("KEY", readBuffer)) {
      completed++;
      char strb[3];
      strb[2] = '\0';
      for (int i = 0; i < (strlen(tmp+1)/2); i++)
      {
        strb[0] = *(tmp+1+(i*2));
        strb[1] = *(tmp+2+(i*2));
        secInfo.key.rgbKey[i] = strtoul(strb, NULL, 16);
      }
    } else if (!strcmp("KEYCHAR", readBuffer)) {
      completed++;
      strcpy((char*)secInfo.key.rgbKey, (tmp+1));
    } else if (!strcmp("KEYINDEX", readBuffer)) {
      completed++;
      secInfo.key.index = strtoul(tmp+1, NULL, 10);
    }

    if (completed == 4)
      break;
  }
}
#endif //CKEASYWIFICONFIG_SUPPRESS_SD

// To be called once at the beginning
bool CKEasyWifiConfig::Setup()
{
  int conID = DWIFIcK::INVALID_CONNECTION_ID;
  DNETcK::STATUS status;

  #ifndef CKEASYWIFICONFIG_SUPPRESS_SD
    if (typeConfig == SD_FILE)
    {
      ReadConfigFile();
    }
  #endif  //CKEASYWIFICONFIG_SUPPRESS_SD
    
  if (typeConfig == SEC_INFO || typeConfig == SD_FILE)
  {
    // Use connection parameters provided in secInfo
    #ifdef CKEASYWIFICONFIG_SERIAL_DEBUG
      Serial << "EASYWIFI: Attempting connection to WiFi network --> " << ssid << endl;
    #endif

    #if !defined(CKEASYWIFICONFIG_SUPPRESS_SECINFO) || !defined(CKEASYWIFICONFIG_SUPPRESS_SD)
    switch(secInfo.securityType)
    {
      case DWIFIcK::WF_SECURITY_WPA2_WITH_KEY :
      case DWIFIcK::WF_SECURITY_WPA_AUTO_WITH_KEY :
      case DWIFIcK::WF_SECURITY_WPA_WITH_KEY :
        conID = DWIFIcK::connect(ssid, secInfo.key.wpa, &status);
        break;
      case DWIFIcK::WF_SECURITY_WPA2_WITH_PASS_PHRASE :
      case DWIFIcK::WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE :
        conID = DWIFIcK::connect(ssid, secInfo.key.szPassPhrase, &status);
        break;
      case DWIFIcK::WF_SECURITY_WPA_WITH_PASS_PHRASE :
        conID = DWIFIcK::connect(DWIFIcK::WF_SECURITY_WPA_WITH_PASS_PHRASE, ssid, secInfo.key.szPassPhrase, &status);
        break;
      case DWIFIcK::WF_SECURITY_WEP_40 :
        conID = DWIFIcK::connect(ssid, secInfo.key.wep40, secInfo.key.index, &status);
        break;
      case DWIFIcK::WF_SECURITY_WEP_104 :
        conID = DWIFIcK::connect(ssid, secInfo.key.wep104, secInfo.key.index, &status);
        break;
      case DWIFIcK::WF_SECURITY_OPEN :
      default:
        conID = DWIFIcK::connect(ssid, &status);
    }
    #endif  //defined(CKEASYWIFICONFIG_SUPPRESS_SECINFO) || defined(CKEASYWIFICONFIG_SUPPRESS_SD)
  }
  else
  {
    // Use connection parameters in WF_Config.x
    #ifdef CKEASYWIFICONFIG_SERIAL_DEBUG
      Serial << "EASYWIFI: Attempting connection to WiFi network with parameters from WF_Config.x" << endl;
    #endif
    
    #ifndef CKEASYWIFICONFIG_SUPPRESS_WF_CONFIG
      conID = DWIFIcK::connect(0, &status);
    #endif  //CKEASYWIFICONFIG_SUPPRESS_WF_CONFIG
  }
  
  if(conID != DWIFIcK::INVALID_CONNECTION_ID)
  {
    #ifdef CKEASYWIFICONFIG_SERIAL_DEBUG
      Serial << "EASYWIFI: Connection Created, ConID = " << _DEC(conID) << endl;
    #endif
    
    if (useDhcp)
    {
      // intialize the stack with DHCP
      DNETcK::begin();
    }
    else
    {
      // intialize the stack with a static IP
      DNETcK::begin(staticIp);
    }
    
    if(DNETcK::isInitialized(connectionTimeout, &status))
    {
      #ifdef CKEASYWIFICONFIG_SERIAL_DEBUG
        Serial << "EASYWIFI: IP Stack Initialized." << endl;
        IPv4 myIp;
        if (DNETcK::getMyIP(&myIp))
        {
            Serial << "EASYWIFI: IP Adress : " << _DEC(myIp.rgbIP[0]) << "." << _DEC(myIp.rgbIP[1]) << "." << _DEC(myIp.rgbIP[2]) << "." << _DEC(myIp.rgbIP[3]) << endl;
        }
      #endif
      // We have a valid connection with a valid IP Stack
      return true;
    }
    else if(DNETcK::isStatusAnError(status))
    {
      #ifdef CKEASYWIFICONFIG_SERIAL_DEBUG
        Serial << "EASYWIFI: Error in initializing TCP/IP stack, status: " << _DEC(status) << endl;
      #endif
    }
  }
  else
  {
    #ifdef CKEASYWIFICONFIG_SERIAL_DEBUG
      Serial << "EASYWIFI: Unable to begin the connection process to the WIFI network, status: " << _DEC(status) << endl;
    #endif
  }
  return false;
}

#endif /* CKEASYWIFICONFIG_H_ */
