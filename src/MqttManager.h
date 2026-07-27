#pragma once

#include <WiFi.h>
#include <PubSubClient.h>

class MqttManager
{
public:

    MqttManager();

    void begin();

    void loop();

    bool connected() const;

    void publishState();

private:

    void reconnect();

    void handleMessage(
        char* topic,
        byte* payload,
        unsigned int length
    );

    void publishDiscovery();

    void publishDiscoveryEntity(
        const String& uniqueId,
        const String& name,
        const String& stateTopic,
        const String& commandTopic,
        const String& icon,
        int minValue,
        int maxValue
    );

private:

    WiFiClient wifiClient;

    PubSubClient mqttClient;
};