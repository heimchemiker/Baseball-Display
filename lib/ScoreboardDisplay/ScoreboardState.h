#pragma once

#include <stdint.h>

struct ScoreboardState
{
    uint8_t batter = 0;

    uint8_t balls = 0;
    uint8_t strikes = 0;
    uint8_t outs = 0;

    uint8_t inningsA[10] = {0};
    uint8_t inningsB[10] = {0};

    uint8_t runsA = 0;
    uint8_t runsB = 0;

    uint8_t hitsA = 0;
    uint8_t hitsB = 0;

    uint8_t errorsA = 0;
    uint8_t errorsB = 0;
};