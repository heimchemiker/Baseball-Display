Unterstützte JSON-Updates
Balls setzen:
JSON
{
    "balls":23
}

Strikes setzen:
JSON
{
    "strikes":13
}

Outs setzen:
JSON
{
    "outs":23
}

Batter setzen:
JSON
{
    "atBat":233
}

Komplettes Team A aktualisieren:
JSON
{
    "inningsA":[1,0,2,0,3,0,0,1,0,0],
    "hitsA":12,
    "errorsA":17
}

Ein Inning ändern:
JSON
{
    "team":"A",
    "inning":3,
    "value":55
}

Hit erhöhen:
JSON
{
    "team":"A",
    "stat":"hits",
    "delta":15
}

Error verringern:
JSON
{
    "team":"B",
    "stat":"errors",
    "delta":-15
}