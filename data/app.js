let ws;

let currentState = {};

function connectWebSocket()
{
    const protocol =
        location.protocol === "https:"
            ? "wss:"
            : "ws:";

    ws = new WebSocket(
        `${protocol}//${window.location.host}/ws`
    );

    ws.onopen = () =>
    {
        console.log("WebSocket verbunden");
    };

    ws.onclose = () =>
    {
        setTimeout(
            connectWebSocket,
            2000
        );
    };

    ws.onmessage = event =>
    {
        currentState =
            JSON.parse(
                event.data
            );

        updatePreview();
        updateControls();
    };
}

connectWebSocket();
