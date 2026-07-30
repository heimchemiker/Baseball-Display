#pragma once

#include <WiFi.h>

class WifiManager
{
public:

    bool begin();

    void loop();

    bool connected() const;

    String ipAddress() const;

    String ssid() const;

    int32_t rssi() const;

    String scanNetworksJson();

private:

    void startAP();

    void startSTA();

    void reconnect();

private:

    uint32_t lastReconnectAttempt = 0;
};