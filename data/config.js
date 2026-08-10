let currentConfig = {};

async function loadConfig()
{
    try
    {
        const response =
            await fetch("/api/config");

        if(!response.ok)
        {
            throw new Error(
                "Config konnte nicht geladen werden"
            );
        }

        currentConfig =
            await response.json();

        fillForm(currentConfig);
    }
    catch(error)
    {
        console.error(error);

        showStatus(
            "Fehler beim Laden der Konfiguration",
            true
        );
    }
}

function fillForm(cfg)
{
    setValue(
        "wifiSSID",
        cfg.wifiSSID
    );

    setValue(
        "wifiPassword",
        cfg.wifiPassword
    );

    setValue(
        "mqttHost",
        cfg.mqttHost
    );

    setValue(
        "mqttPort",
        cfg.mqttPort
    );

    setValue(
        "mqttUser",
        cfg.mqttUser
    );

    setValue(
        "mqttPassword",
        cfg.mqttPassword
    );

    setValue(
        "ntpServer",
        cfg.ntpServer
    );

    setValue(
        "theme",
        cfg.theme || "dark"
    );

    setValue(
        "ntpEnabled",
        String(cfg.ntpEnabled)
    );

    setValue(
        "brightnessSensor",
        String(cfg.brightnessSensor)
    );

    setValue(
        "luxMin",
        cfg.luxMin
    );

    setValue(
        "luxMax",
        cfg.luxMax
    );

    setValue(
        "brightnessMin",
        cfg.brightnessMin
    );

    setValue(
        "brightnessMax",
        cfg.brightnessMax
    );

    setColor(
        "colorGlobal",
        cfg.colorGlobal
    );

    setColor(
        "colorTeamA",
        cfg.colorTeamA
    );

    setColor(
        "colorTeamB",
        cfg.colorTeamB
    );

    setColor(
        "colorClock",
        cfg.colorClock
    );

    setColor(
        "colorBalls",
        cfg.colorBalls
    );

    setColor(
        "colorStrikes",
        cfg.colorStrikes
    );

    setColor(
        "colorOuts",
        cfg.colorOuts
    );

    setColor(
        "highlightCurrentInning",
        cfg.highlightCurrentInning
    );

    updateThemePreview();
}

function setValue(id, value)
{
    const element =
        document.getElementById(id);

    if(element)
    {
        element.value =
            value ?? "";
    }
}

function setColor(id, value)
{
    const element =
        document.getElementById(id);

    if(element && value)
    {
        element.value = value;
    }
}

async function scanWifi()
{
    try
    {
        // Start scan
        const startResp = await fetch("/api/wifi/scan", { method: "POST" });

        if(!startResp.ok)
        {
            throw new Error("Scan start failed");
        }

        // Poll for results
        const timeout = 10000; // 10s
        const interval = 500;
        let elapsed = 0;
        let networks = null;

        while(elapsed < timeout)
        {
            await new Promise(r => setTimeout(r, interval));
            elapsed += interval;

            const res = await fetch("/api/wifi/scan/result");

            if(!res.ok) continue;

            const body = await res.text();

            try
            {
                const parsed = JSON.parse(body);

                if(parsed.available === false)
                {
                    continue;
                }

                networks = parsed;
                break;
            }
            catch(e)
            {
                // If body is not a JSON object with available flag, assume it's the array
                networks = JSON.parse(body);
                break;
            }
        }

        if(!networks)
        {
            throw new Error("No scan results");
        }

        const list =
            document.getElementById(
                "wifiList"
            );

        list.innerHTML = "";

        networks.sort(
            (a,b) =>
                b.rssi - a.rssi
        );

        networks.forEach(
            network =>
            {
                const option =
                    document.createElement(
                        "option"
                    );

                option.value =
                    network.ssid;

                option.text =
                    `${network.ssid} (${network.rssi} dBm)`;

                list.appendChild(
                    option
                );
            });

        showStatus(`${networks.length} Netzwerke gefunden`);
    }
    catch(error)
    {
        console.error(error);

        showStatus(
            "Fehler beim WLAN-Scan",
            true
        );
    }
}

function buildConfigObject()
{
    return {

        wifiSSID:
            document.getElementById(
                "wifiSSID"
            ).value,

        wifiPassword:
            document.getElementById(
                "wifiPassword"
            ).value,

        mqttHost:
            document.getElementById(
                "mqttHost"
            ).value,

        mqttPort:
            parseInt(
                document.getElementById(
                    "mqttPort"
                ).value
            ),

        mqttUser:
            document.getElementById(
                "mqttUser"
            ).value,

        mqttPassword:
            document.getElementById(
                "mqttPassword"
            ).value,

        ntpEnabled:
            document.getElementById(
                "ntpEnabled"
            ).value === "true",

        ntpServer:
            document.getElementById(
                "ntpServer"
            ).value,

        theme:
            document.getElementById(
                "theme"
            ).value,

        colorGlobal:
            document.getElementById(
                "colorGlobal"
            ).value,

        colorTeamA:
            document.getElementById(
                "colorTeamA"
            ).value,

        colorTeamB:
            document.getElementById(
                "colorTeamB"
            ).value,

        colorClock:
            document.getElementById(
                "colorClock"
            ).value,

        colorBalls:
            document.getElementById(
                "colorBalls"
            ).value,

        colorStrikes:
            document.getElementById(
                "colorStrikes"
            ).value,

        colorOuts:
            document.getElementById(
                "colorOuts"
            ).value,

        highlightCurrentInning:
            document.getElementById(
                "highlightCurrentInning"
            ).value,

        brightnessSensor:
            document.getElementById(
                "brightnessSensor"
            ).value === "true",

        luxMin:
            parseFloat(
                document.getElementById(
                    "luxMin"
                ).value
            ),

        luxMax:
            parseFloat(
                document.getElementById(
                    "luxMax"
                ).value
            ),

        brightnessMin:
            parseInt(
                document.getElementById(
                    "brightnessMin"
                ).value
            ),

        brightnessMax:
            parseInt(
                document.getElementById(
                    "brightnessMax"
                ).value
            )
    };
}

async function saveConfig()
{
    try
    {
        const config =
            buildConfigObject();

        const response =
            await fetch(
                "/api/config",
                {
                    method: "POST",
                    headers:
                    {
                        "Content-Type":
                            "application/json"
                    },
                    body:
                        JSON.stringify(
                            config
                        )
                });

        if(!response.ok)
        {
            throw new Error();
        }

        showStatus(
            "Konfiguration gespeichert"
        );
    }
    catch(error)
    {
        console.error(error);

        showStatus(
            "Fehler beim Speichern",
            true
        );
    }
}

async function restartDevice()
{
    if(
        !confirm(
            "ESP32 neu starten?"
        )
    )
    {
        return;
    }

    try
    {
        await fetch(
            "/api/restart",
            {
                method:"POST"
            }
        );

        showStatus(
            "Neustart ausgelöst"
        );
    }
    catch(error)
    {
        console.error(error);

        showStatus(
            "Neustart fehlgeschlagen",
            true
        );
    }
}

function showStatus(
    text,
    isError = false
)
{
    const status =
        document.getElementById(
            "status"
        );

    if(!status)
    {
        return;
    }

    status.innerText =
        text;

    status.className =
        isError
            ? "error"
            : "success";
}

function updateThemePreview()
{
    const theme =
        document.getElementById(
            "theme"
        ).value;

    if(theme === "light")
    {
        document.body.classList.add(
            "light-theme"
        );
    }
    else
    {
        document.body.classList.remove(
            "light-theme"
        );
    }
}

document.addEventListener(
    "DOMContentLoaded",
    () =>
    {
        loadConfig();

        const wifiList =
            document.getElementById(
                "wifiList"
            );

        if(wifiList)
        {
            wifiList.addEventListener(
                "change",
                function()
                {
                    document
                        .getElementById(
                            "wifiSSID"
                        )
                        .value =
                            this.value;
                });
        }

        const theme =
            document.getElementById(
                "theme"
            );

        if(theme)
        {
            theme.addEventListener(
                "change",
                updateThemePreview
            );
        }

        const form =
            document.getElementById(
                "configForm"
            );

        if(form)
        {
            form.addEventListener(
                "submit",
                function(event)
                {
                    event.preventDefault();

                    saveConfig();
                }
            );
        }
    });