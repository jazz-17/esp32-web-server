// mac_sniffer.h
#ifndef MAC_SNIFFER_H
#define MAC_SNIFFER_H

#include "esp_wifi.h"
#include <WString.h>
#include "Arduino.h"

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
