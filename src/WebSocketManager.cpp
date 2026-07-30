#include "WebSocketManager.h"

#include <ArduinoJson.h>

#include "StateSerializer.h"
#include "ScoreboardDisplay.h"

extern ScoreboardDisplay scoreboard;

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