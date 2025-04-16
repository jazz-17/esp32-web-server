#include "http_reporting.h"
#include "target_devices.h"
#include "config.h"
#include <WiFi.h> // Needed for WiFiClient
#include <HTTPClient.h>
// #include <ArduinoJson.h> // Optional: For more complex JSON payloads

void reportTargetDevice(const String &macAddress, const String &deviceName)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Cannot report: WiFi not connected.");
        return;
    }

    Serial.print("Reporting presence of: ");
    Serial.print(deviceName);
    Serial.print(" (");
    Serial.print(macAddress);
    Serial.println(")");

    WiFiClient client; // Use WiFiClient for ESP32 standard HTTPClient
    HTTPClient http;

    String serverUrl = "http://" + String(SERVER_IP) + ":" + String(SERVER_PORT) + String(SERVER_PATH);

    if (DEBUG_MODE)
    {
        Serial.print("POST to: ");
        Serial.println(serverUrl);
    }

    if (http.begin(client, serverUrl))
    { // Use the version with WiFiClient
        http.addHeader("Content-Type", "application/json");

        // Construct JSON payload (simple example)
        // For more complex JSON, consider using the ArduinoJson library
        String jsonPayload = "{\"macAddress\":\"" + macAddress + "\"}";

        if (DEBUG_MODE)
        {
            Serial.print("Payload: ");
            Serial.println(jsonPayload);
        }

        int httpResponseCode = http.POST(jsonPayload);

        if (httpResponseCode > 0)
        {
            Serial.printf("HTTP Response code: %d\n", httpResponseCode);
            String payload = http.getString();
            Serial.print("Response: ");
            Serial.println(payload);
            // Mark as reported ONLY if the server confirmed (e.g., HTTP 200 OK)
            if (httpResponseCode == HTTP_CODE_OK)
            {
                markTargetDeviceReported(macAddress); // Reset the flag in target_devices
            }
            else
            {
                Serial.printf("Server returned status %d, report NOT marked as complete.\n", httpResponseCode);
            }
        }
        else
        {
            Serial.printf("HTTP POST failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
            // Do not mark as reported on failure, it will be retried next interval
        }
        http.end();
    }
    else
    {
        Serial.println("ERROR: Unable to connect to server for HTTP POST.");
    }
}