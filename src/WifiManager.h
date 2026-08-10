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

    // Start an asynchronous WiFi scan in a separate FreeRTOS task.
    // Returns true if the scan was started.
    bool startScanAsync();

    // Check whether a scan result is available.
    bool isScanAvailable() const;

    // Retrieve the last scan JSON result and clear availability.
    String popLastScanJson();

    // Internal helpers used by the scan task
    void setScanInProgress(bool v);
    void setScanAvailable(bool v);
    void setLastScanJson(const String& s);

private:

    void startAP();

    void startSTA();

    void reconnect();

private:

    uint32_t lastReconnectAttempt = 0;
    // Async scan state
    volatile bool scanInProgress = false;
    volatile bool scanAvailable = false;
    String lastScanJson;
};