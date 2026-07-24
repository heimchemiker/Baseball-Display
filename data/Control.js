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
