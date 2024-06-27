
//////////////////////////////
// PARIONA NO QUIERE ESTO >:(
#include <Arduino.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#define BLUE_LED 2

String ssid = "KIKI-1";
String password = "andromeda_17089";

const char *PARAM_INPUT_1 = "ssid";
const char *PARAM_INPUT_2 = "password";
const char *JSON_CONFIG_FILE = "/config.json";
AsyncWebServer server(80);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000; // interval to wait for Wi-Fi connection (milliseconds)

bool setupSPIFFS()
{
  // SPIFFS.format();
  Serial.println("Mounting SPIFFS File System...");

  // May need to make it begin(true) first time you are using SPIFFS
  if (SPIFFS.begin(false) || SPIFFS.begin(true))
  {
    Serial.println("SPIFFS mounted successfully");
    return true;
  }
  Serial.println("Failed to mount SPIFFS filesystem");
  return false;
}

bool getConfigData()
{
  if (!SPIFFS.exists(JSON_CONFIG_FILE))
  {
    Serial.println("The config file does not exist");
    return false;
  }

  Serial.println("Reading config file");
  File configFile = SPIFFS.open(JSON_CONFIG_FILE, "r");
  if (!configFile)
  {
    Serial.println("Failed to open config file");
    return false;
  }

  Serial.println("Opened configuration file");
  JsonDocument json;
  DeserializationError error = deserializeJson(json, configFile);
  serializeJsonPretty(json, Serial);
  configFile.close();

  if (error)
  {
    Serial.println("Failed to load json config");
    return false;
  }

  Serial.println("Parsing JSON");
  ssid = json["ssid"].as<String>();         // Convert JSON values to String
  password = json["password"].as<String>(); // Convert JSON values to String
  Serial.println("SSID: " + String(ssid));
  Serial.println("Password: " + String(password));

  return true;
}

bool getWifiConnection()
{
  Serial.println("Connecting to WiFi..");

  if (ssid == "")
  {
    Serial.println("Undefined SSID");
    return false;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long startMillis = millis();

  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - startMillis >= interval)
    {
      return false;
    }

    if ((millis() - startMillis) % 1000 == 0)
    {
      Serial.print(".");
      delay(100); // Small delay to avoid spamming the serial output
    }
  }

  Serial.println("Connected to the WiFi network");
  digitalWrite(BLUE_LED, HIGH);
  return true;
}

void writeConfigFile()
{
  Serial.println("Writting config file to SPIFFS");
  JsonDocument json;
  json["ssid"] = ssid.c_str();
  json["password"] = password.c_str();

  File configFile = SPIFFS.open(JSON_CONFIG_FILE, "w");
  if (!configFile)
  {
    Serial.println("Failed to open config file for writing");
  }

  serializeJsonPretty(json, Serial);
  if (serializeJson(json, configFile) == 0)
  {
    Serial.println(F("Failed to write to file"));
  }
  else
  {
    Serial.println(F("Config file saved successfully"));
    Serial.print("SSID set to: ");
    Serial.println(ssid);
    Serial.print("Password set to: ");
    Serial.println(password);
  }
  configFile.close();
}

void setup()
{
  Serial.begin(9600);
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BLUE_LED, LOW);

  if (!setupSPIFFS())
  {
    return;
  }

  if (getConfigData() && getWifiConnection())
  {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { 
    Serial.print("Received request from client with IP: ");
    Serial.println(request->client()->remoteIP());  
                request->send(200, "text/plain", "hello world"); });

    // Start server
    Serial.println("Starting server...");
    Serial.println(WiFi.localIP());
    server.begin();
  }
  else
  {
    Serial.println("\nFailed to connect to WiFi. Initializing AP (Access Point).");

    if (!WiFi.softAP("ESP32-AP", NULL))
    {
      Serial.println("AP failed to start");
      return;
    }
    Serial.println("AP SSID: ESP32-AP");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/admin.html", "text/html"); });
    server.on("/toggle", HTTP_GET, [](AsyncWebServerRequest *request)
              { 
                digitalWrite(BLUE_LED, !digitalRead(BLUE_LED));
                request->send(200, "text/plain", "toggle"); });

    server.on("/", HTTP_POST, [](AsyncWebServerRequest *request)
              {

      Serial.println("POST request received");
      Serial.println("Initializing WiFi credentials configuration...");
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          //HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();

          }
          //HTTP POST password value
          if (p->name() == PARAM_INPUT_2) {
            password = p->value().c_str();
          }

          Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
    writeConfigFile();
    request->send(200, "text/plain", "Done. ESP will restart");
    delay(3000);
    ESP.restart(); });

    server.begin();
  }
}
void loop()
{
}