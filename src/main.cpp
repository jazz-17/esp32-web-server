#include <WiFi.h>
#include <esp_wifi.h> // For core wifi functions if needed directly

#include "config.h"         // Your configurations
#include "sniffer.h"        // Sniffer functions and data structures
#include "target_devices.h" // Target device list management
#include "http_reporting.h" // HTTP reporting functions

bool isConnected = false;
unsigned long lastReportTime = 0;
unsigned long lastPruneTime = 0; // Timer for pruning old MACs

// --- Button Configuration ---
const int BUTTON_PIN = 13; // GPIO pin the button is connected to

// --- Debounce Variables ---
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
unsigned long debounceDelay = 50;   // the debounce time in milliseconds; increase if you get multiple clicks

// --- Button State Variables ---
// We need to track the current debounced state and the state from the previous loop iteration
int buttonState = HIGH;     // The current *debounced* state of the button (HIGH = not pressed with internal pullup)
int lastButtonState = HIGH; // The previous *debounced* reading from the input pin

void check_myButton();
void setup_myButton();

void setup()
{
  Serial.begin(115200);
  Serial.println("\n\n--- ESP32 MAC Sniffer & Reporter ---");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // LED OFF initially

  setup_myButton();          // Call the button setup
  initializeTargetDevices(); // Load the list of devices we care about
  setupSniffer();

  // --- Connect to WiFi First ---
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_CONNECT_TIMEOUT)
  {
    Serial.print(".");
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Blink LED during connection attempt
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    isConnected = true;
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Connected on Channel: ");
    int currentChannel = WiFi.channel();
    Serial.println(currentChannel);
    digitalWrite(LED_PIN, HIGH); // LED ON solid when connected
  }
  else
  {
    isConnected = false;
    Serial.println("\nFailed to connect to WiFi.");
    digitalWrite(LED_PIN, LOW); // LED OFF
    // Sniffer will not be started. Maybe enter a deep sleep or retry?
    Serial.println("Cannot start sniffer without WiFi connection. System idle.");
  }

  lastReportTime = millis();
  lastPruneTime = millis();
}

void loop()
{
  check_myButton(); // Call the button check function repeatedly

  unsigned long currentTime = millis();

  // --- Handle WiFi Connection State Changes ---
  if (WiFi.status() != WL_CONNECTED && isConnected)
  {
    Serial.println("WiFi disconnected!");
    digitalWrite(LED_PIN, LOW);
    isConnected = false;
    stopSniffer(); // Stop sniffing if connection lost
                   // Add reconnection logic here if desired
  }
  else if (WiFi.status() == WL_CONNECTED && !isConnected)
  {
    // This handles reconnection after a drop
    Serial.println("WiFi reconnected!");
    isConnected = true;
    digitalWrite(LED_PIN, HIGH);
  }

  // --- Periodic Tasks (Non-Blocking) ---
  esp_goto_next_channel();
  delay(1000);

  // 1. Check for and report target devices
  if (isConnected && (currentTime - lastReportTime >= REPORT_INTERVAL))
  {
    lastReportTime = currentTime;
    if (DEBUG_MODE)
      Serial.println("Checking for target devices to report...");

    const std::vector<TargetDevice> &targets = getTargetDevices();
    for (const auto &target : targets)
    {

      // Check the flag set by the sniffer callback via markTargetDeviceForReporting
      if (target.reportPending && !target.registered)
      {
        Serial.print("Alumno ");
        Serial.print(target.name);
        Serial.println(" detectado...");
        if (!DEBUG_MODE)
        {
          reportTargetDevice(target.macAddress, target.name);
        }
        // Attempt to send HTTP report (function now handles WiFi check too)
        // Note: reportTargetDevice now calls markTargetDeviceReported *only* on success.
      }
    }
  }

  // 2. Prune old MAC addresses from the detected list
  if (MAC_EXPIRY_TIME > 0 && (currentTime - lastPruneTime >= MAC_EXPIRY_TIME))
  { // Prune at least as often as expiry time
    lastPruneTime = currentTime;
    pruneOldMacs();
  }

  // --- Keep loop responsive ---
  // Add delay(1) or yield() if you experience watchdog resets with very busy loops,
  // but generally avoid blocking delays. The sniffer callback runs in the background.
  // delay(1);
}

// --- Setup ---
void setup_myButton()
{
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.println("Button ready. Press the button connected to GPIO " + String(BUTTON_PIN));
  buttonState = digitalRead(BUTTON_PIN);
  lastButtonState = buttonState;
}
void check_myButton()
{

  // 1. Read the current physical state of the button
  int reading = digitalRead(BUTTON_PIN);

  // 2. Check if the physical state has changed (potential bounce or real press/release)
  // If it differs from the last reading, reset the debounce timer
  if (reading != lastButtonState)
  {
    lastDebounceTime = millis();
  }

  // 3. Check if enough time has passed since the last potential state change
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    // Whatever the reading is at this point, it's considered stable (debounced)

    // 4. Check if the stable state is different from the previously accepted debounced state
    if (reading != buttonState)
    {
      buttonState = reading; // Update the accepted debounced state

      // 5. Perform action ONLY on the transition from HIGH to LOW (button pressed)
      if (buttonState == LOW)
      {
        Serial.println("Button Pressed! (Single Action)");
        // --- PUT YOUR SINGLE ACTION CODE HERE ---
        // Example: blink an LED once, send one MQTT message, etc.
        // digitalWrite(LED_BUILTIN, HIGH);
        // delay(100); // Short delay for visual feedback if blinking LED
        // digitalWrite(LED_BUILTIN, LOW);
        // ----------------------------------------
      }
      else
      {
        // Optional: Action when the button is released (transition from LOW to HIGH)
        // Serial.println("Button Released!");
      }
    }
  }

  // 6. Save the current physical reading for the next loop iteration
  lastButtonState = reading;
}
