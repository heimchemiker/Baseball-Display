#include "MqttManager.h"

#include <ArduinoJson.h>

#include "ScoreboardDisplay.h"
#include "ConfigManager.h"
#include "WebSocketManager.h"

extern ScoreboardDisplay scoreboard;
extern ConfigManager configManager;
extern WebSocketManager websocketManager;

static MqttManager* instance = nullptr;

static void mqttCallback(
    char* topic,
    byte* payload,
    unsigned int length)
{
    if(instance)
    {
        instance->handleMessage(
            topic,
            payload,
            length
        );
    }
}

MqttManager::MqttManager()
:
mqttClient(wifiClient)
{
}

void MqttManager::begin()
{
    instance = this;

    mqttClient.setServer(
        configManager.config().mqttHost.c_str(),
        configManager.config().mqttPort
    );

    mqttClient.setCallback(
        mqttCallback
    );
}

bool MqttManager::connected() const
{
    return mqttClient.connected();
}

void MqttManager::loop()
{
    if(!mqttClient.connected())
    {
        reconnect();
    }

    mqttClient.loop();
}

void MqttManager::reconnect()
{
    if(mqttClient.connected())
    {
        return;
    }

    String clientId =
        "BaseballScoreboard-";

    clientId +=
        String((uint32_t)ESP.getEfuseMac(), HEX);

    bool success;

    if(configManager.config().mqttUser.length())
    {
        success =
            mqttClient.connect(
                clientId.c_str(),
                configManager.config().mqttUser.c_str(),
                configManager.config().mqttPassword.c_str()
            );
    }
    else
    {
        success =
            mqttClient.connect(
                clientId.c_str()
            );
    }

    if(!success)
    {
        return;
    }

    mqttClient.subscribe(
        "baseball/cmd"
    );

    mqttClient.subscribe(
        "baseball/balls/set"
    );

    mqttClient.subscribe(
        "baseball/strikes/set"
    );

    mqttClient.subscribe(
        "baseball/outs/set"
    );

    mqttClient.subscribe(
        "baseball/batter/set"
    );

    for(int inning=1;
        inning<=10;
        inning++)
    {
        String topicA =
            "baseball/teama/inning/" +
            String(inning) +
            "/set";

        String topicB =
            "baseball/teamb/inning/" +
            String(inning) +
            "/set";

        mqttClient.subscribe(
            topicA.c_str()
        );

        mqttClient.subscribe(
            topicB.c_str()
        );
    }

    mqttClient.subscribe(
        "baseball/teama/hits/set"
    );

    mqttClient.subscribe(
        "baseball/teama/errors/set"
    );

    mqttClient.subscribe(
        "baseball/teamb/hits/set"
    );

    mqttClient.subscribe(
        "baseball/teamb/errors/set"
    );

    publishState();
}

void MqttManager::publishState()
{
    JsonDocument doc;

    auto& state =
        scoreboard.state();

    doc["atBat"] =
        state.batter;

    doc["balls"] =
        state.balls;

    doc["strikes"] =
        state.strikes;

    doc["outs"] =
        state.outs;

    doc["runsA"] =
        state.runsA;

    doc["runsB"] =
        state.runsB;

    doc["hitsA"] =
        state.hitsA;

    doc["hitsB"] =
        state.hitsB;

    doc["errorsA"] =
        state.errorsA;

    doc["errorsB"] =
        state.errorsB;

    JsonArray inningsA =
        doc["inningsA"].to<JsonArray>();

    JsonArray inningsB =
        doc["inningsB"].to<JsonArray>();

    for(int i=0;i<10;i++)
    {
        inningsA.add(
            state.inningsA[i]
        );

        inningsB.add(
            state.inningsB[i]
        );
    }

    String json;

    serializeJson(
        doc,
        json
    );

    mqttClient.publish(
        "baseball/state",
        json.c_str(),
        true
    );
}

void MqttManager::handleMessage(
    char* topic,
    byte* payload,
    unsigned int length)
{
    String value;

    for(size_t i=0;
        i<length;
        i++)
    {
        value +=
            (char)payload[i];
    }

    auto& state =
        scoreboard.state();

    String t(topic);

    if(t == "baseball/balls/set")
    {
        state.balls =
            constrain(
                value.toInt(),
                0,
                3
            );
    }

    else if(t == "baseball/strikes/set")
    {
        state.strikes =
            constrain(
                value.toInt(),
                0,
                2
            );
    }

    else if(t == "baseball/outs/set")
    {
        state.outs =
            constrain(
                value.toInt(),
                0,
                2
            );
    }

    else if(t == "baseball/batter/set")
    {
        state.batter =
            constrain(
                value.toInt(),
                0,
                99
            );
    }

    else if(t.startsWith(
        "baseball/teama/inning/"))
    {
        int inning =
            t.substring(
                22,
                t.lastIndexOf('/')
            ).toInt();

        if(inning >= 1 &&
           inning <= 10)
        {
            state.inningsA[
                inning-1
            ] =
            constrain(
                value.toInt(),
                0,
                99
            );
        }
    }

    else if(t.startsWith(
        "baseball/teamb/inning/"))
    {
        int inning =
            t.substring(
                22,
                t.lastIndexOf('/')
            ).toInt();

        if(inning >= 1 &&
           inning <= 10)
        {
            state.inningsB[
                inning-1
            ] =
            constrain(
                value.toInt(),
                0,
                99
            );
        }
    }

    else if(t ==
        "baseball/teama/hits/set")
    {
        state.hitsA =
            value.toInt();
    }

    else if(t ==
        "baseball/teama/errors/set")
    {
        state.errorsA =
            value.toInt();
    }

    else if(t ==
        "baseball/teamb/hits/set")
    {
        state.hitsB =
            value.toInt();
    }

    else if(t ==
        "baseball/teamb/errors/set")
    {
        state.errorsB =
            value.toInt();
    }

    else if(t == "baseball/cmd")
    {
        JsonDocument cmd;

        if(deserializeJson(
            cmd,
            value) == DeserializationError::Ok)
        {
            if(cmd["action"] ==
               "resetCount")
            {
                state.balls = 0;
                state.strikes = 0;
                state.outs = 0;
            }
        }
    }

    scoreboard.render();

    websocketManager.broadcastState();

    publishState();
}