window.ws = null;

window.currentState = {};

function connectWebSocket()
{
    const protocol =
        location.protocol === "https:"
            ? "wss:"
            : "ws:";

    window.ws = new WebSocket(
        `${protocol}//${window.location.host}/ws`
    );

    window.ws.onopen = () =>
    {
        console.log("WebSocket verbunden");

        // Update UI status element if present
        const st = document.getElementById('status');
        if(st)
        {
            st.textContent = 'WebSocket verbunden';
            st.className = 'success';
        }
    };

    window.ws.onclose = (ev) =>
    {
        window.ws = null;

        const st = document.getElementById('status');
        if(st)
        {
            st.textContent = `WebSocket getrennt (${ev.code})`;
            st.className = 'error';
        }

        console.error('WebSocket closed', ev.code, ev.reason, ev.wasClean);
        setTimeout(connectWebSocket, 2000);
    };

    window.ws.onerror = (ev) =>
    {
        window.ws = null;

        const st = document.getElementById('status');
        if(st)
        {
            st.textContent = 'WebSocket Fehler';
            st.className = 'error';
        }
        console.error('WebSocket error', ev);
    };

    window.ws.onmessage = event =>
    {
        window.currentState =
            JSON.parse(
                event.data
            );

        updatePreview();
        updateControls();
    };
}

connectWebSocket();
