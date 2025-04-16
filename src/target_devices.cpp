#include "target_devices.h"
#include "config.h" // Include config for DEBUG_MODE if needed for messages

// Define the list of target devices here
std::vector<TargetDevice> targetDeviceList = {
    {"celular_REDMI", "24:D3:37:13:60:60", false, false},
    {"LENOVO", "94:08:53:98:65:EB", false, false},
    {"pc1", "3E:95:09:BF:2F:57", false, false},
    {"pc2", "4E:95:09:BF:2F:57", false, false}
    // Add more devices as needed
};

void initializeTargetDevices()
{
    // Convert MAC addresses to uppercase for consistent comparison
    for (auto &device : targetDeviceList)
    {
        device.macAddress.toUpperCase();
        device.reportPending = false; // Ensure flag is reset on init
    }
    if (DEBUG_MODE)
    {
        Serial.print("Initialized ");
        Serial.print(targetDeviceList.size());
        Serial.println(" target devices.");
    }
}

const std::vector<TargetDevice> &getTargetDevices()
{
    return targetDeviceList;
}

// Finds a target MAC and sets its reportPending flag to true
bool markTargetDeviceForReporting(const String &detectedMac)
{
    for (auto &target : targetDeviceList)
    {
        if (target.macAddress == detectedMac)
        {
            if (!target.reportPending)
            { // Only mark if not already pending
                if (DEBUG_MODE)
                {
                    Serial.print("Target device matched: ");
                    Serial.println(target.name);
                }
                target.reportPending = true;
                return true;
            }
            return false; // Found, but already marked
        }
    }
    return false; // Not found
}

// Check if a specific target needs reporting
bool needsReporting(const String &targetMac)
{
    for (auto &target : targetDeviceList)
    {
        if (target.macAddress == targetMac && target.reportPending)
        {
            return true;
        }
    }
    return false;
}

// Marks a target MAC as reported (resets the flag)
void markTargetDeviceReported(const String &targetMac)
{
    for (auto &target : targetDeviceList)
    {
        if (target.macAddress == targetMac)
        {
            target.reportPending = false;
            target.registered = true;
            if (DEBUG_MODE)
            {
                Serial.print("Marked target as reported: ");
                Serial.println(target.name);
            }
            return;
        }
    }
}