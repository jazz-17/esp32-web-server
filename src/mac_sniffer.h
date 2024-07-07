// mac_sniffer.h
#ifndef MAC_SNIFFER_H
#define MAC_SNIFFER_H

#include "esp_wifi.h"
#include <WiFi.h>
#include "Arduino.h"
#include <HTTPClient.h>


// Function declarations
void setupMacSniffer();
void updateTimer();
void findMyMACs();
void sniffer(void *buf, wifi_promiscuous_pkt_type_t type);
void esp_goto_next_channel();

// Extern declarations for global variables
extern int channel;
extern int timer;
extern bool debugMode; //hi

#endif // MAC_SNIFFER_H
