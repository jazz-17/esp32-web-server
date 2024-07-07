// #include "mac_sniffer.h"
#include "network_config.h"

// #define LED 2

// void setup()
// {
//     Serial.begin(115200);
//     pinMode(LED, OUTPUT);
//     digitalWrite(LED, LOW);

//     setupMacSniffer();

//     WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
//     Serial.println("iniciando");
//     Serial.println("Escaneando MACs...");
// }

// bool isConnected = false;
// void loop()
// {

//     /**** wifi code***/
//     if (WiFi.status() == WL_CONNECTED && !isConnected)
//     {
//         Serial.print("Conectado a la red WiFi: ");
//         Serial.println(WIFI_SSID);
//         digitalWrite(LED, HIGH);
//         isConnected = true;
//     }
//     else if (WiFi.status() != WL_CONNECTED && isConnected)
//     {
//         Serial.println("Desconectado de la red WiFi");
//         digitalWrite(LED, LOW);
//         isConnected = false;
//     }

//     /****mac_sniffer code****/
//     // if (channel > 14)
//     //     channel = 1;
//     //esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
//     esp_goto_next_channel();
//     delay(1000);
//     findMyMACs();
//     //channel++;
// }

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define LED_BUILTIN 2
#define BUTTON_A 33
#define BUTTON_B 25
#define BUTTON_C 26
#define BUTTON_D 27

void mostrarPregunta();
void manejarPulsaciones();
void registrarRespuesta(String respuesta);
void enviarRespuesta(int pregunta, String respuesta);
void mostrarResultados();

const char *ssid = "KIKI-1";
const char *password = "andromeda_1708";
// const char* serverName = "http://tu_dominio.com/guardar_respuesta.php";
const char *serverName = "http://proyecto.test/guardar_respuesta.php";

const char *preguntas[] = {
    "Pregunta 1",
    "Pregunta 2",
    "Pregunta 3",
    "Pregunta 4",
    "Pregunta 5",
    "Pregunta 6",
    "Pregunta 7",
    "Pregunta 8",
    "Pregunta 9",
    "Pregunta 10"};

const char *opciones[] = {
    "A B C D",
    "A B C D",
    "A B C D",
    "A B C D",
    "A B C D",
    "A B C D",
    "A B C D",
    "A B C D",
    "A B C D",
    "A B C D"};

int preguntaActual = 0;
String respuestas[10];

void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  lcd.init();
  lcd.backlight();
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(BUTTON_D, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
  digitalWrite(LED_BUILTIN, HIGH);

  mostrarPregunta();
}

void loop()
{
  //manejarPulsaciones();
  enviarRespuesta(preguntaActual, "A");
  delay(5000);
  preguntaActual++;
}

void mostrarPregunta()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(preguntas[preguntaActual]);
  lcd.setCursor(0, 1);
  lcd.print(opciones[preguntaActual]);
}

void manejarPulsaciones()
{
  if (digitalRead(BUTTON_A) == LOW)
  {
    registrarRespuesta("A");
  }
  else if (digitalRead(BUTTON_B) == LOW)
  {
    registrarRespuesta("B");
  }
  else if (digitalRead(BUTTON_C) == LOW)
  {
    registrarRespuesta("C");
  }
  else if (digitalRead(BUTTON_D) == LOW)
  {
    registrarRespuesta("D");
  }
}

void registrarRespuesta(String respuesta)
{
  Serial.print("Respuesta ");
  Serial.print(respuesta);
  Serial.println(" seleccionada");
  respuestas[preguntaActual] = respuesta;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Respuesta: ");
  lcd.print(respuesta);
  lcd.setCursor(0, 1);
  lcd.print("Confirmado");
  delay(2000);

  enviarRespuesta(preguntaActual + 1, respuesta);

  preguntaActual++;
  if (preguntaActual < 10)
  {
    mostrarPregunta();
  }
  else
  {
    mostrarResultados();
  }
}

void enviarRespuesta(int pregunta, String respuesta)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Registrando asistencia...");
    WiFiClient client;
    HTTPClient http;
    String serverIP = SERVER_IP;
    String serverPath = "http://" + serverIP + "/examen/guardar-respuesta";
    http.begin(client, serverPath.c_str());
    http.addHeader("Content-Type", "application/json");

    String jsonPayLoad = "{\"pregunta\": " + String(pregunta) + ", \"respuesta\": \"" + respuesta + "\"}";
    int httpResponseCode = http.POST(jsonPayLoad);

    if (httpResponseCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      String payload = http.getString();
      Serial.print("Respuesta: ");
      Serial.println(payload);
    }
    else
    {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
  else
  {
    Serial.println("Desconectado de WiFi");
  }
}

void mostrarResultados()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Examen Completo");
  delay(2000);

  for (int i = 0; i < 10; i++)
  {
    Serial.print("Pregunta ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(preguntas[i]);
    Serial.print(" - Respuesta: ");
    Serial.println(respuestas[i]);
    delay(1000);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Resultados en ");
  lcd.setCursor(0, 1);
  lcd.print("Serial");
}
