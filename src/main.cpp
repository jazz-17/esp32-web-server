#include "mac_sniffer.h"
#include <WiFi.h>

#define WIFI_SSID "KIKI-1"
#define WIFI_PASSWORD "andromeda_1708"
#define SERVER_URL "http://192.168.0.15:3000/";
#define LED 2

void setup()
{
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    setupMacSniffer();
    debugMode = false;

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("iniciando");
}

bool isConnected = false;
void loop()
{

    // wifi code
    if (WiFi.status() == WL_CONNECTED && !isConnected)
    {
        Serial.println("Conectado");
        digitalWrite(LED, HIGH);
        isConnected = true;
    }
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(".");
        digitalWrite(LED, !digitalRead(LED));
        delay(750);
        isConnected = false;
    }

    // mac code
    if (channel > 14)
        channel = 1;
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
    delay(1000);
    if (timer > 0)
        updateTimer();
    if (debugMode == false)
        showMyMACs();
    channel++;
}
