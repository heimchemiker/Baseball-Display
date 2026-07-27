#include "WebSocketManager.h"

extern WebSocketManager websocketManager;
extern ScoreboardDisplay scoreboard;

state.balls = json["balls"];

//after each change in score
scoreboard.render();
websocketManager.broadcastState();