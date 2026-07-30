#pragma once

#include <ESPAsyncWebServer.h>

class WebSocketManager
{
public:

    WebSocketManager();

    void begin(
        AsyncWebServer& server
    );

    void broadcastState();

    AsyncWebSocket& socket();

private:

    void handleMessage(
        AsyncWebSocketClient* client,
        const String& message
    );

    AsyncWebSocket ws;
};