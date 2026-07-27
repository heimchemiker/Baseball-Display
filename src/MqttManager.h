#pragma once

#include <WiFi.h>
#include <PubSubClient.h>

class MqttManager
{
public:

    void begin();

    void loop();

    void publishState();

    bool connected() const;

private:

    void reconnect();

    void handleMessage(
        char* topic,
        byte* payload,
        unsigned int length
    );

    WiFiClient wifiClient;

    PubSubClient mqttClient;
};