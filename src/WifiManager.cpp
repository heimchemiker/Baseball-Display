#include "WifiManager.h"

#include <WiFi.h>
#include <ArduinoJson.h>

#include "ConfigManager.h"

extern ConfigManager configManager;

#define DEFAULT_AP_SSID "BaseballScoreboard"
#define DEFAULT_AP_PASS "scoreboard123"

bool WifiManager::begin()
{
    WiFi.disconnect(true, true);

    delay(500);

    bool enableAP =
        configManager.config().apMode;

    bool enableSTA =
        configManager.config().staMode;

    if(enableAP && enableSTA)
    {
        WiFi.mode(WIFI_AP_STA);

        startAP();
        startSTA();
    }
    else if(enableAP)
    {
        WiFi.mode(WIFI_AP);

        startAP();
    }
    else if(enableSTA)
    {
        WiFi.mode(WIFI_STA);

        startSTA();
    }
    else
    {
        WiFi.mode(WIFI_AP);

        startAP();
    }

    return true;
}

void WifiManager::startAP()
{
    WiFi.softAP(
        DEFAULT_AP_SSID,
        DEFAULT_AP_PASS
    );

    Serial.println();
    Serial.println("Access Point gestartet");
    Serial.print("SSID: ");
    Serial.println(DEFAULT_AP_SSID);
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());
}

void WifiManager::startSTA()
{
    String ssid =
        configManager.config().wifiSSID;

    String password =
        configManager.config().wifiPassword;

    if(ssid.isEmpty())
    {
        Serial.println(
            "Keine WLAN-Konfiguration vorhanden"
        );
        return;
    }

    Serial.print("Verbinde mit ");
    Serial.println(ssid);

    WiFi.begin(
        ssid.c_str(),
        password.c_str()
    );

    uint32_t start =
        millis();

    while(
        WiFi.status() != WL_CONNECTED &&
        millis() - start < 15000
    )
    {
        delay(250);
        Serial.print(".");
    }

    Serial.println();

    if(WiFi.status() == WL_CONNECTED)
    {
        Serial.println(
            "WLAN verbunden"
        );

        Serial.print("IP: ");
        Serial.println(
            WiFi.localIP()
        );
    }
    else
    {
        Serial.println(
            "WLAN Verbindung fehlgeschlagen"
        );
    }
}

void WifiManager::loop()
{
    if (
        !configManager.config().staMode
    )
    {
        return;
    }

    if(
        configManager.config().wifiSSID.isEmpty()
    )
    {
        return;
    }
    
    if(
        configManager.config().staMode &&
        WiFi.status() != WL_CONNECTED
    )
    {
        reconnect();
    }
}

void WifiManager::reconnect()
{
    if(
        configManager.config().wifiSSID.isEmpty()
    )
    {
        return;
    }

    uint32_t now =
        millis();

    if(
        now - lastReconnectAttempt <
        30000
    )
    {
        return;
    }

    lastReconnectAttempt = now;

    Serial.println(
        "WLAN Reconnect ..."
    );

    WiFi.disconnect();

    delay(500);

    WiFi.begin(
        configManager.config()
            .wifiSSID.c_str(),
        configManager.config()
            .wifiPassword.c_str()
    );
}

bool WifiManager::connected() const
{
    return
        WiFi.status() ==
        WL_CONNECTED;
}

String WifiManager::ipAddress() const
{
    if(
        WiFi.status() ==
        WL_CONNECTED
    )
    {
        return
            WiFi.localIP()
                .toString();
    }

    return
        WiFi.softAPIP()
            .toString();
}

String WifiManager::ssid() const
{
    if(
        WiFi.status() ==
        WL_CONNECTED
    )
    {
        return WiFi.SSID();
    }

    return DEFAULT_AP_SSID;
}

int32_t WifiManager::rssi() const
{
    if(
        WiFi.status() ==
        WL_CONNECTED
    )
    {
        return WiFi.RSSI();
    }

    return 0;
}

String WifiManager::scanNetworksJson()
{
    JsonDocument doc;

    JsonArray array =
        doc.to<JsonArray>();

    int count =
        WiFi.scanNetworks();

    for(
        int i = 0;
        i < count;
        i++
    )
    {
        JsonObject network =
            array.add<JsonObject>();

        network["ssid"] =
            WiFi.SSID(i);

        network["rssi"] =
            WiFi.RSSI(i);

        network["channel"] =
            WiFi.channel(i);

        network["encryption"] =
            WiFi.encryptionType(i);

        //network["hidden"] =
        //    WiFi.isHidden(i);
    }

    String json;

    serializeJson(
        doc,
        json
    );

    return json;
}

// Async scan task
static void wifiScanTask(void* pv)
{
    WifiManager* mgr = static_cast<WifiManager*>(pv);

    if(!mgr)
    {
        vTaskDelete(NULL);
        return;
    }

    mgr->setScanInProgress(true);
    mgr->setScanAvailable(false);

    int count = WiFi.scanNetworks();


    // Build JSON array manually to avoid deprecated DynamicJsonDocument.
    String json = "[";

    for(int i = 0; i < count; i++)
    {
        if(i > 0) json += ",";

        String ssid = WiFi.SSID(i);

        json += "{\"ssid\":\"";

        // Escape backslashes and quotes in SSID
        for(size_t k = 0; k < ssid.length(); k++)
        {
            char c = ssid[k];
            if(c == '"' || c == '\\')
            {
                json += '\\';
                json += c;
            }
            else if(c == '\n')
            {
                json += "\\n";
            }
            else
            {
                json += c;
            }
        }

        json += "\",\"rssi\":";
        json += WiFi.RSSI(i);
        json += ",\"channel\":";
        json += WiFi.channel(i);
        json += ",\"encryption\":";
        json += WiFi.encryptionType(i);
        json += "}";
    }

    json += "]";

    mgr->setLastScanJson(json);
    mgr->setScanAvailable(true);
    mgr->setScanInProgress(false);

    vTaskDelete(NULL);
}

bool WifiManager::startScanAsync()
{
    if(scanInProgress)
    {
        return false;
    }

    BaseType_t res = xTaskCreate(
        wifiScanTask,
        "wifiScanTask",
        8192,
        this,
        1,
        NULL
    );

    return res == pdPASS;
}

bool WifiManager::isScanAvailable() const
{
    return scanAvailable;
}

String WifiManager::popLastScanJson()
{
    String tmp = lastScanJson;
    lastScanJson = "";
    scanAvailable = false;
    return tmp;
}

void WifiManager::setScanInProgress(bool v)
{
    scanInProgress = v;
}

void WifiManager::setScanAvailable(bool v)
{
    scanAvailable = v;
}

void WifiManager::setLastScanJson(const String& s)
{
    lastScanJson = s;
}