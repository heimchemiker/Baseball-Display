#include "WebSocketManager.h"
#include getStateJson();

extern String getStateJson();

WebSocketManager::WebSocketManager()
: ws("/ws")
{
}

void WebSocketManager::begin(
    AsyncWebServer& server)
{
    ws.onEvent(
        AsyncWebSocket *server,
           AsyncWebSocketClient *client,
           AwsEventType type,
           void *arg,
           uint8_t *data,
           size_t len
        {
            if(type == WS_EVT_CONNECT)
            {
                client->text(
                    getStateJson()
                );
            }
        });

    server.addHandler(&ws);
}

void WebSocketManager::broadcastState()
{
    ws.textAll(
        getStateJson()
    );
}

AsyncWebSocket&
WebSocketManager::socket()
{
    return ws;
}
