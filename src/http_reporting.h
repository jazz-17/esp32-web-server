#ifndef HTTP_REPORTING_H
#define HTTP_REPORTING_H

#include <Arduino.h>

// Function to send report for a specific MAC address
void reportTargetDevice(const String &macAddress, const String &deviceName);
void esp_goto_next_channel();
#endif // HTTP_REPORTING_H