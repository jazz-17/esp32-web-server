#include "mac_sniffer.h"



unsigned long lastTime = 0;
// 10 minutos 600000;
// 5 segundos 5000
unsigned long timerDelay = 10000;

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
    Serial.println("Escaneando MACs...");
}

bool isConnected = false;
void loop()
{

    // wifi code
    if (WiFi.status() == WL_CONNECTED && !isConnected)
    {
        Serial.print("Conectado a red WiFi con IP: ");
        Serial.println(WiFi.localIP());
        digitalWrite(LED, HIGH);
        isConnected = true;
    }
    else if (WiFi.status() != WL_CONNECTED && isConnected)
    {
        Serial.println("Desconectado de red WiFi");
        digitalWrite(LED, LOW);
        isConnected = false;
    }
    else if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(".");
        digitalWrite(LED, !digitalRead(LED));
        delay(750);
    }



    // macsniffer code
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