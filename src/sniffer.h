#ifndef SNIFFER_H
#define SNIFFER_H

#include <Arduino.h>
#include <esp_wifi.h>
#include <map>
#include <WiFi.h>

// Structure to store information about detected MACs
struct DetectedMacInfo
{
    unsigned long lastSeenMs; // Timestamp when last seen
    int channel;              // Channel where detected (will be fixed in this setup)
};

// Callback function for promiscuous mode
void snifferCallback(void *buf, wifi_promiscuous_pkt_type_t type);

// Function to initialize the sniffer (sets up promiscuous mode)
// Requires WiFi to be connected first to get the channel.
bool setupSniffer();

// Function to stop the sniffer
void stopSniffer();

// Function to get the map of detected MACs (read-only access recommended)
const std::map<String, DetectedMacInfo> &getDetectedMacs();

// Function to periodically prune old MAC entries
void pruneOldMacs();

// Helper to convert MAC byte array to String
String macToString(const uint8_t *mac);

#endif // SNIFFER_H