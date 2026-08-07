#pragma once

#include <Arduino.h>

struct AppConfig
{
    String wifiSSID;
    String wifiPassword;

    bool apMode;
    bool staMode;

    String mqttHost;
    uint16_t mqttPort;

    String mqttUser;
    String mqttPassword;

    bool ntpEnabled;
    String ntpServer;

    String theme;

    String colorGlobal;

    String colorTeamA;
    String colorTeamB;

    String colorClock;

    String colorBalls;
    String colorStrikes;
    String colorOuts;

    String highlightCurrentInning;

    bool brightnessSensor;

    float luxMin;
    float luxMax;

    uint8_t brightnessMin;
    uint8_t brightnessMax;
    uint8_t fixedBrightness;
};

class ConfigManager
{
public:

    bool begin();

    bool load();

    bool save();

    String toJson();

    bool fromJson(
        const String& json
    );

    AppConfig& config();

    uint8_t currentBrightness() const;
    void setCurrentBrightness(uint8_t value);

private:

    void createDefaults();

private:

    AppConfig cfg;

    uint8_t currentBrightnessValue = 64;
};