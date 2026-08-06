#include "ScoreboardDisplay.h"

ScoreboardDisplay::ScoreboardDisplay(
    Adafruit_NeoPixel* topStrip,
    Adafruit_NeoPixel* teamAStrip,
    Adafruit_NeoPixel* teamBStrip)
:
top(topStrip),
teamA(teamAStrip),
teamB(teamBStrip),
topDisplay(topStrip),
displayA(teamAStrip),
displayB(teamBStrip)
{
}

void ScoreboardDisplay::begin()
{
    top->begin();
    teamA->begin();
    teamB->begin();

    colorTeamA =
        top->Color(255,0,0);

    colorTeamB =
        top->Color(0,255,0);

    colorClock =
        top->Color(255,255,255);
}

ScoreboardState&
ScoreboardDisplay::state()
{
    return scoreboardState;
}

void ScoreboardDisplay::setClock(
    uint8_t hour,
    uint8_t minute)
{
    currentHour = hour;
    currentMinute = minute;
}

void ScoreboardDisplay::setCurrentInning(
    uint8_t inning)
{
    currentInning = inning;
}

uint8_t ScoreboardDisplay::getCurrentInning() const
{
    return currentInning;
}

void ScoreboardDisplay::calculateTotals()
{
    scoreboardState.runsA = 0;
    scoreboardState.runsB = 0;

    for(uint8_t i=0;i<10;i++)
    {
        scoreboardState.runsA +=
            scoreboardState.inningsA[i];

        scoreboardState.runsB +=
            scoreboardState.inningsB[i];
    }
}

void ScoreboardDisplay::render()
{
    calculateTotals();

    top->clear();
    teamA->clear();
    teamB->clear();

    topDisplay.drawNumber(
        0,
        2,
        scoreboardState.batter,
        colorClock
    );

    topDisplay.drawIndicator(
        28,
        scoreboardState.balls,
        3,
        colorTeamB
    );

    topDisplay.drawIndicator(
        32,
        scoreboardState.strikes,
        2,
        top->Color(255,255,0)
    );

    topDisplay.drawIndicator(
        35,
        scoreboardState.outs,
        2,
        colorTeamA
    );

    topDisplay.drawTime(
        40,
        currentHour,
        currentMinute,
        colorClock
    );

    for(uint8_t i=0;i<10;i++)
    {
        displayA.drawNumber(
            i*14,
            2,
            scoreboardState.inningsA[i],
            colorTeamA
        );

        displayB.drawNumber(
            i*14,
            2,
            scoreboardState.inningsB[i],
            colorTeamB
        );
    }

    top->show();
    teamA->show();
    teamB->show();
}