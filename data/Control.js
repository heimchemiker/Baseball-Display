function apiUpdate(data)
{
    fetch(
        "/api/state",
        {
            method:"POST",
            headers:
            {
                "Content-Type":
                "application/json"
            },
            body:
                JSON.stringify(
                    data
                )
        });
}

function applyTheme(theme)
{
    const root =
        document.documentElement;

    if(theme === "light")
    {
        root.style.setProperty(
            "--bg-color",
            "#f4f4f4"
        );

        root.style.setProperty(
            "--panel-color",
            "#ffffff"
        );

        root.style.setProperty(
            "--text-color",
            "#111111"
        );

        root.style.setProperty(
            "--segment-off",
            "#dddddd"
        );
    }
    else
    {
        root.style.setProperty(
            "--bg-color",
            "#111111"
        );

        root.style.setProperty(
            "--panel-color",
            "#000000"
        );

        root.style.setProperty(
            "--text-color",
            "#ffffff"
        );

        root.style.setProperty(
            "--segment-off",
            "#220000"
        );
    }
}

function setBalls(value)
{
    apiUpdate({
        balls:value
    });
}

function changeBalls(delta)
{
    apiUpdate({
        balls:
            Math.max(
                0,
                Math.min(
                    3,
                    currentState.balls + delta
                )
            )
    });
}

function setStrikes(value)
{
    apiUpdate({
        strikes:value
    });
}

function changeStrikes(delta)
{
    apiUpdate({
        strikes:
            Math.max(
                0,
                Math.min(
                    2,
                    currentState.strikes + delta
                )
            )
    });
}

function setOuts(value)
{
    apiUpdate({
        outs:value
    });
}

function changeOuts(delta)
{
    apiUpdate({
        outs:
            Math.max(
                0,
                Math.min(
                    2,
                    currentState.outs + delta
                )
            )
    });
}

function createInningControls(
    containerId,
    team
)
{
    const div =
        document.getElementById(
            containerId
        );

    let html =
        `<h2>${team}</h2>`;

    for(let inning=0;
        inning<10;
        inning++)
    {
        html += `
        <div class="inningBlock">

            <button
                onclick="
                  updateInning(
                    '${team}',
                    ${inning},
                    1
                  )">
                +
            </button>

            <input
              id="${team}_${inning}"
              readonly>

            <button
                onclick="
                  updateInning(
                    '${team}',
                    ${inning},
                    -1
                  )">
                -
            </button>

        </div>
        `;
    }

    html += `
    <div class="stats">

    H
    <button onclick="
      updateStat(
      '${team}',
      'hits',
      -1)">
      -
    </button>

    <button onclick="
      updateStat(
      '${team}',
      'hits',
      1)">
      +
    </button>

    E
    <button onclick="
      updateStat(
      '${team}',
      'errors',
      -1)">
      -
    </button>

    <button onclick="
      updateStat(
      '${team}',
      'errors',
      1)">
      +
    </button>

    </div>
    `;

    div.innerHTML = html;
}

function updateInning(
    team,
    inning,
    delta)
{
    const values =
        team === "A"
        ? currentState.inningsA
        : currentState.inningsB;

    let newVal =
        values[inning] + delta;

    if(newVal < 0)
        newVal = 0;

    if(newVal > 99)
        newVal = 99;

    apiUpdate({
        team:team,
        inning:inning,
        value:newVal
    });
}

function updateStat(
    team,
    stat,
    delta)
{
    apiUpdate({
        team:team,
        stat:stat,
        delta:delta
    });
}

function updateControls()
{
    if(!currentState)
        return;

    for(let i=0;i<10;i++)
    {
        const a =
            document.getElementById(
                `A_${i}`
            );

        if(a)
        {
            a.value =
                currentState
                .inningsA[i];
        }

        const b =
            document.getElementById(
                `B_${i}`
            );

        if(b)
        {
            b.value =
                currentState
                .inningsB[i];
        }
    }

    document
        .getElementById(
            "batter"
        )
        .value =
            currentState.atBat;
}