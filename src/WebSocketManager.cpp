#include "WebSocketManager.h"

#include <ArduinoJson.h>

#include "StateSerializer.h"
#include "ScoreboardDisplay.h"

extern ScoreboardDisplay scoreboard;
extern MqttManager mqttManager;

WebSocketManager::WebSocketManager()
:
ws("/ws")
{
}

void WebSocketManager::begin(
    AsyncWebServer& server)
{
    ws.onEvent(
        [this]
        (
            AsyncWebSocket* server,
            AsyncWebSocketClient* client,
            AwsEventType type,
            void* arg,
            uint8_t* data,
            size_t len
        )
        {
            switch(type)
            {
                case WS_EVT_CONNECT:
                {
                    Serial.printf(
                        "[WS] Client #%u verbunden\n",
                        client->id()
                    );

                    client->text(
                        getStateJson()
                    );

                    break;
                }

                case WS_EVT_DISCONNECT:
                {
                    Serial.printf(
                        "[WS] Client #%u getrennt\n",
                        client->id()
                    );

                    break;
                }

                case WS_EVT_PONG:
                {
                    break;
                }

                case WS_EVT_ERROR:
                {
                    Serial.printf(
                        "[WS] Fehler Client #%u\n",
                        client->id()
                    );

                    break;
                }

                case WS_EVT_DATA:
                {
                    AwsFrameInfo* info =
                        (AwsFrameInfo*)arg;

                    if(
                        info->opcode == WS_TEXT &&
                        info->final &&
                        info->index == 0
                    )
                    {
                        String message;

                        for(size_t i = 0;
                            i < len;
                            i++)
                        {
                            message +=
                                (char)data[i];
                        }

                        handleMessage(
                            client,
                            message
                        );
                    }

                    break;
                }
            }
        });

    server.addHandler(&ws);
}

AsyncWebSocket&
WebSocketManager::socket()
{
    return ws;
}

void WebSocketManager::broadcastState()
{
    ws.textAll(
        getStateJson()
    );
}

void WebSocketManager::handleMessage(
    AsyncWebSocketClient* client,
    const String& message)
{
    JsonDocument doc;

    auto result =
        deserializeJson(
            doc,
            message
        );

    if(result)
    {
        client->text(
            "{\"error\":\"invalid json\"}"
        );
        return;
    }

    auto& state =
        scoreboard.state();

    if(doc["atBat"].is<int>())
    {
        state.batter =
            constrain(
                doc["atBat"],
                0,
                99
            );
    }

    if(doc["balls"].is<int>())
    {
        state.balls =
            constrain(
                doc["balls"],
                0,
                3
            );
    }

    if(doc["strikes"].is<int>())
    {
        state.strikes =
            constrain(
                doc["strikes"],
                0,
                2
            );
    }

    if(doc["outs"].is<int>())
    {
        state.outs =
            constrain(
                doc["outs"],
                0,
                2
            );
    }

    if(doc["hitsA"].is<int>())
    {
        state.hitsA =
            constrain(
                doc["hitsA"],
                0,
                99
            );
    }

    if(doc["hitsB"].is<int>())
    {
        state.hitsB =
            constrain(
                doc["hitsB"],
                0,
                99
            );
    }

    if(doc["errorsA"].is<int>())
    {
        state.errorsA =
            constrain(
                doc["errorsA"],
                0,
                99
            );
    }

    if(doc["errorsB"].is<int>())
    {
        state.errorsB =
            constrain(
                doc["errorsB"],
                0,
                99
            );
    }

    if(doc["currentInning"].is<int>())
    {
        scoreboard.setCurrentInning(
            constrain(
                doc["currentInning"],
                1,
                10
            )
        );
    }

    if(doc["inningsA"].is<JsonArray>())
    {
        JsonArray arr =
            doc["inningsA"];

        for(uint8_t i = 0;
            i < arr.size() && i < 10;
            i++)
        {
            state.inningsA[i] =
                constrain(
                    arr[i].as<int>(),
                    0,
                    99
                );
        }
    }

    if(doc["inningsB"].is<JsonArray>())
    {
        JsonArray arr =
            doc["inningsB"];

        for(uint8_t i = 0;
            i < arr.size() && i < 10;
            i++)
        {
            state.inningsB[i] =
                constrain(
                    arr[i].as<int>(),
                    0,
                    99
                );
        }
    }

    if(doc["cmd"].is<String>())
    {
        String cmd =
            doc["cmd"];

        if(cmd == "resetCount")
        {
            state.balls = 0;
            state.strikes = 0;
            state.outs = 0;
        }

        if(cmd == "clearGame")
        {
            memset(
                state.inningsA,
                0,
                sizeof(state.inningsA)
            );

            memset(
                state.inningsB,
                0,
                sizeof(state.inningsB)
            );

            state.hitsA = 0;
            state.hitsB = 0;

            state.errorsA = 0;
            state.errorsB = 0;

            state.balls = 0;
            state.strikes = 0;
            state.outs = 0;
        }
    }

    scoreboard.render();
    broadcastState();
    mqttManager.publishState();
}