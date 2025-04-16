#ifndef CONFIG_H
#define CONFIG_H

// --- WiFi Network Configuration ---
#define WIFI_SSID "CALLUPE"        // Replace with your WiFi SSID
#define WIFI_PASSWORD "ABC123456"  // Replace with your WiFi Password
#define WIFI_CONNECT_TIMEOUT 10000 // Max time to wait for WiFi connection (ms)

// --- Server Configuration ---
#define SERVER_IP "YOUR_SERVER_IP_OR_HOSTNAME" // Replace with your server's IP or hostname
#define SERVER_PORT 80                         // Replace with your server's port if not 80
#define SERVER_PATH "/asistencia/ingresar"     // The path for the POST request

// --- Sniffer Configuration ---
#define LED_PIN 2              // GPIO pin for the status LED
#define REPORT_INTERVAL 10000  // How often to check for target MACs and report (ms)
#define MAC_EXPIRY_TIME 300000 // How long to keep a detected MAC in memory if not seen again (ms) - 5 minutes. Set to 0 to disable expiry.
#define MAX_DETECTED_MACS 100  // Maximum number of unique MACs to store (adjust based on RAM)

// --- Application Settings ---
#define DEBUG_MODE true // Set to true for more detailed Serial output, false for operational mode

#endif // CONFIG_H