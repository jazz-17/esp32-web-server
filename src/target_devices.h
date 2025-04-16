#ifndef TARGET_DEVICES_H
#define TARGET_DEVICES_H

#include <Arduino.h>
#include <vector>

struct TargetDevice
{
    String name;
    String macAddress;  // Stored uppercase, no colons for easier comparison later if needed
    bool reportPending; // Flag to indicate if a report is needed for this device
    bool registered;    // Flag to indicate if a report was already made for the session
};

// Function to initialize the list of target devices
void initializeTargetDevices();

// Function to get the list of target devices (read-only access)
const std::vector<TargetDevice> &getTargetDevices();

// Function to find a target device by MAC and mark it for reporting
// Returns true if found and marked, false otherwise
bool markTargetDeviceForReporting(const String &detectedMac);

// Function to check if a specific target needs reporting
bool needsReporting(const String &targetMac);

// Function to mark a target device as reported
void markTargetDeviceReported(const String &targetMac);

#endif // TARGET_DEVICES_H