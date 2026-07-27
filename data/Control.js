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