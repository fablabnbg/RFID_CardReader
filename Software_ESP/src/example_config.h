#ifndef EXAMPLE_CONFIG
#define EXAMPLE_CONFIG
#include <ESP8266Wifi.h>
#include "CommunicationHandler.h"
/// EXAMPLE Config: copy file to config.h and adapt values
//CONFIG: Read config from EEPROM or SPIFFS
//const char ap_ssid[] = "IBims1WLAN";
//const char ap_password[] = "1234_istkeinsicheresPasswort";
//const IPAddress Server(192,168,178,20);
const char ap_ssid[] = "FabLab_NBG";
const char ap_password[] = "fablabnbg";
const IPAddress Server(172,16,21,169);
//const char ap_ssid[] = "NETGEAR";
//const char ap_password[] = "1234istkeinsicherespasswort";
//const IPAddress Server(192,168,0,7);
const int Port = 55411;
const int Timeout_Val = 60000; //60s
const int Timeout_Critical = 10000; //10s before
const int Timeout_Block = 5000; //5s block after card was granted
//const int Timeout_Val = 3600000; //1h in milliseconds
//const int Timeout_Critical = 60000; //1min before off
const char Sender[64] = {RFID_SENDER_DRILL_NAME};

#endif
