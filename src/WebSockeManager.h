#pragma once

#include <ESPAsyncWebServer.h>

class WebSocketManager
{
public:
    WebSocketManager();

    void begin(AsyncWebServer& server);

    void broadcastState();

    AsyncWebSocket& socket();

private:
    AsyncWebSocket ws;
};
