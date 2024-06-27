// mac_sniffer.h
#ifndef MAC_SNIFFER_H
#define MAC_SNIFFER_H

#include "esp_wifi.h"
#include <WString.h>
#include <WiFi.h>
#include "Arduino.h"
#include <HTTPClient.h>


#define WIFI_SSID "KIKI-1"
//#define WIFI_SSID "KIKI-portable"
#define WIFI_PASSWORD "andromeda_1708"
#define SERVER_URL "http://192.168.0.15:3000/";

// Function declarations
void setupMacSniffer();
void updateTimer();
void showMyMACs();
void sniffer(void *buf, wifi_promiscuous_pkt_type_t type);

// Extern declarations for global variables
extern int channel;
extern int timer;
extern bool debugMode;

#endif // MAC_SNIFFER_H
