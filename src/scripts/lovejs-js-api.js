var love_send_event = Module.cwrap('JS_send_event', 'number', ['string', 'string', 'number'])
{
    // example to make sure signals work
    if (typeof WorkerGlobalScope !== 'undefined' && self instanceof WorkerGlobalScope) {
        // "in worker"
    }
    else
    {
        var canvas = document.getElementById("canvas");
        canvas.addEventListener (
            "echo",
            (e )=> {
                if (e.detail) {
                    love_send_event("echo", JSON.stringify(e.detail), 0);
                }
                else{
                    love_send_event("echo", "", 0);
                }
            },
            false
        );
    }
}
