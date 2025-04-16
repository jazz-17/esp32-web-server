#include "sniffer.h"
#include "config.h"
#include "target_devices.h" // To mark devices when detected

// Use std::map for efficient MAC lookup and storage
// Key: MAC Address (String, uppercase, no colons), Value: DetectedMacInfo
std::map<String, DetectedMacInfo> detectedMacs;
const wifi_promiscuous_filter_t filt = {
    .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA};
int channel = 1;
// --- WiFi Packet Structures (from Espressif docs, simplified) ---
typedef struct
{
    unsigned frame_ctrl : 16;
    unsigned duration_id : 16;
    uint8_t addr1[6]; /* receiver address */
    uint8_t addr2[6]; /* sender address */
    uint8_t addr3[6]; /* filtering address */
    unsigned sequence_ctrl : 16;
    uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct
{
    wifi_ieee80211_mac_hdr_t hdr;
    uint8_t payload[0]; /* network data ended with 4 bytes csum (FCS) */
} wifi_ieee80211_packet_t;
// --- End Packet Structures ---

// Promiscuous mode callback - CORRECTED VERSION
void snifferCallback(void *buf, wifi_promiscuous_pkt_type_t type)
{
    wifi_promiscuous_pkt_t *promiscuousPkt = (wifi_promiscuous_pkt_t *)buf;
    wifi_ieee80211_packet_t *pkt = (wifi_ieee80211_packet_t *)promiscuousPkt->payload;
    wifi_ieee80211_mac_hdr_t *hdr = &pkt->hdr;

    // Extract Source MAC Address (addr2)
    String mac = macToString(hdr->addr2);
    unsigned long now = millis();
    int currentChannel = channel; // Get the channel we are currently fixed on

    // --- C++11/14 compatible find-and-update/insert logic ---
    auto it = detectedMacs.find(mac); // Search for the MAC address

    if (it != detectedMacs.end())
    {
        // MAC Found: Update the timestamp of the existing entry
        it->second.lastSeenMs = now;
    }
    else
    {
        // MAC Not Found: Insert a new entry if space allows

        // Check for overflow *before* inserting
        if (detectedMacs.size() >= MAX_DETECTED_MACS)
        {
            // Handle overflow: remove the element considered "first" by map order (simple strategy)
            // Note: Map iteration order isn't strictly oldest, but it's a basic way to make space.
            if (!detectedMacs.empty())
            {
                detectedMacs.erase(detectedMacs.begin());
                Serial.println("WARN: Detected MACs list full, removed oldest/first entry.");
            }
            else
            {
                // Should not happen if size >= MAX_DETECTED_MACS > 0, but safety check
                Serial.println("ERROR: MAC list full but map is empty? Cannot insert.");
                return; // Cannot insert
            }
        }

        // Now it's safe to insert the new MAC using emplace (constructs in-place)
        detectedMacs.emplace(mac, DetectedMacInfo{now, currentChannel});

        // --- Actions for a newly detected MAC ---
        if (DEBUG_MODE)
        {
            Serial.print("New MAC detected: ");
            Serial.print(mac);
            Serial.print(" on channel ");
            Serial.println(currentChannel);
        }
        // Check if this newly detected MAC is one of our targets
        markTargetDeviceForReporting(mac);
        // --- End actions for new MAC ---
    }
    // --- End C++11/14 compatible logic ---
}

// Initialize promiscuous mode ON THE CURRENT WIFI CHANNEL
bool setupSniffer()
{
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_start();
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_filter(&filt);
    esp_wifi_set_promiscuous_rx_cb(&snifferCallback);
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    // esp_wifi_start(); // Restart WiFi if it was stopped

    Serial.println("Sniffer initialized successfully.");
    return true;
}

// Stop the sniffer
void stopSniffer()
{
    esp_wifi_set_promiscuous(false);
    esp_wifi_set_promiscuous_rx_cb(nullptr); // De-register callback
    Serial.println("Sniffer stopped.");
}

// Get detected MACs (const reference to prevent modification)
const std::map<String, DetectedMacInfo> &getDetectedMacs()
{
    return detectedMacs;
}

// Remove old entries
void pruneOldMacs()
{
    if (MAC_EXPIRY_TIME <= 0)
        return; // Expiry disabled

    unsigned long pruneBefore = millis() - MAC_EXPIRY_TIME;
    int count = 0;
    for (auto it = detectedMacs.begin(); it != detectedMacs.end(); /* no increment here */)
    {
        if (it->second.lastSeenMs < pruneBefore)
        {
            it = detectedMacs.erase(it); // Erase and get iterator to next element
            count++;
        }
        else
        {
            ++it; // Only increment if not erased
        }
    }
    if (count > 0 && DEBUG_MODE)
    {
        Serial.printf("Pruned %d old MAC entries.\n", count);
    }
}

void esp_goto_next_channel()
{
    channel++;
    if (channel > 14)
        channel = 1;
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

// Helper: Convert MAC bytes to String (XX:XX:XX:XX:XX:XX)
String macToString(const uint8_t *mac)
{
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return String(macStr);
}