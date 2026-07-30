#include "ConfigManager.h"

#include <ArduinoJson.h>
#include <LittleFS.h>

#define CONFIG_FILE "/config.json"

bool ConfigManager::begin()
{
    if(!LittleFS.begin(true))
    {
        return false;
    }

    if(!LittleFS.exists(CONFIG_FILE))
    {
        createDefaults();
        return save();
    }

    return load();
}

void ConfigManager::createDefaults()
{
    cfg.wifiSSID = "";
    cfg.wifiPassword = "";

    cfg.apMode = true;
    cfg.staMode = true;

    cfg.mqttHost = "";
    cfg.mqttPort = 1883;

    cfg.mqttUser = "";
    cfg.mqttPassword = "";

    cfg.ntpEnabled = true;
    cfg.ntpServer = "pool.ntp.org";

    cfg.theme = "dark";

    cfg.colorGlobal = "#FF0000";

    cfg.colorTeamA = "#FF0000";
    cfg.colorTeamB = "#00FF00";

    cfg.colorClock = "#FFFFFF";

    cfg.colorBalls = "#00FF00";
    cfg.colorStrikes = "#FFFF00";
    cfg.colorOuts = "#FF0000";

    cfg.highlightCurrentInning = "#FFFFFF";

    cfg.brightnessSensor = true;

    cfg.luxMin = 0.0f;
    cfg.luxMax = 1000.0f;

    cfg.brightnessMin = 5;
    cfg.brightnessMax = 255;
}

bool ConfigManager::load()
{
    File file =
        LittleFS.open(
            CONFIG_FILE,
            "r"
        );

    if(!file)
    {
        createDefaults();
        return save();
    }

    String json =
        file.readString();

    file.close();

    return fromJson(json);
}

bool ConfigManager::save()
{
    File file =
        LittleFS.open(
            CONFIG_FILE,
            "w"
        );

    if(!file)
    {
        return false;
    }

    file.print(
        toJson()
    );

    file.close();

    return true;
}

String ConfigManager::toJson()
{
    JsonDocument doc;

    doc["wifiSSID"] =
        cfg.wifiSSID;

    doc["wifiPassword"] =
        cfg.wifiPassword;

    doc["apMode"] =
        cfg.apMode;

    doc["staMode"] =
        cfg.staMode;

    doc["mqttHost"] =
        cfg.mqttHost;

    doc["mqttPort"] =
        cfg.mqttPort;

    doc["mqttUser"] =
        cfg.mqttUser;

    doc["mqttPassword"] =
        cfg.mqttPassword;

    doc["ntpEnabled"] =
        cfg.ntpEnabled;

    doc["ntpServer"] =
        cfg.ntpServer;

    doc["theme"] =
        cfg.theme;

    doc["colorGlobal"] =
        cfg.colorGlobal;

    doc["colorTeamA"] =
        cfg.colorTeamA;

    doc["colorTeamB"] =
        cfg.colorTeamB;

    doc["colorClock"] =
        cfg.colorClock;

    doc["colorBalls"] =
        cfg.colorBalls;

    doc["colorStrikes"] =
        cfg.colorStrikes;

    doc["colorOuts"] =
        cfg.colorOuts;

    doc["highlightCurrentInning"] =
        cfg.highlightCurrentInning;

    doc["brightnessSensor"] =
        cfg.brightnessSensor;

    doc["luxMin"] =
        cfg.luxMin;

    doc["luxMax"] =
        cfg.luxMax;

    doc["brightnessMin"] =
        cfg.brightnessMin;

    doc["brightnessMax"] =
        cfg.brightnessMax;

    String json;

    serializeJsonPretty(
        doc,
        json
    );

    return json;
}

bool ConfigManager::fromJson(
    const String& json)
{
    JsonDocument doc;

    DeserializationError err =
        deserializeJson(
            doc,
            json
        );

    if(err)
    {
        return false;
    }

    cfg.wifiSSID =
        doc["wifiSSID"] | "";

    cfg.wifiPassword =
        doc["wifiPassword"] | "";

    cfg.apMode =
        doc["apMode"] | true;

    cfg.staMode =
        doc["staMode"] | true;

    cfg.mqttHost =
        doc["mqttHost"] | "";

    cfg.mqttPort =
        doc["mqttPort"] | 1883;

    cfg.mqttUser =
        doc["mqttUser"] | "";

    cfg.mqttPassword =
        doc["mqttPassword"] | "";

    cfg.ntpEnabled =
        doc["ntpEnabled"] | true;

    cfg.ntpServer =
        doc["ntpServer"] | "pool.ntp.org";

    cfg.theme =
        doc["theme"] | "dark";

    cfg.colorGlobal =
        doc["colorGlobal"] | "#FF0000";

    cfg.colorTeamA =
        doc["colorTeamA"] | "#FF0000";

    cfg.colorTeamB =
        doc["colorTeamB"] | "#00FF00";

    cfg.colorClock =
        doc["colorClock"] | "#FFFFFF";

    cfg.colorBalls =
        doc["colorBalls"] | "#00FF00";

    cfg.colorStrikes =
        doc["colorStrikes"] | "#FFFF00";

    cfg.colorOuts =
        doc["colorOuts"] | "#FF0000";

    cfg.highlightCurrentInning =
        doc["highlightCurrentInning"] | "#FFFFFF";

    cfg.brightnessSensor =
        doc["brightnessSensor"] | true;

    cfg.luxMin =
        doc["luxMin"] | 0.0;

    cfg.luxMax =
        doc["luxMax"] | 1000.0;

    cfg.brightnessMin =
        doc["brightnessMin"] | 5;

    cfg.brightnessMax =
        doc["brightnessMax"] | 255;

    return true;
}

AppConfig&
ConfigManager::config()
{
    return cfg;
}

uint8_t ConfigManager::currentBrightness() const
{
    return currentBrightnessValue;
}

void ConfigManager::setCurrentBrightness(
    uint8_t value)
{
    currentBrightnessValue = value;
}