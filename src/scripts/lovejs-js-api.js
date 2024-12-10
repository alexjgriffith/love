// love_send_event("eval", "(let [js (require :js)] (js.send-custom-json-event :echo \"{}\"))",0)


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

Module["love_send_event"] = love_send_event;
Module["FS"] = FS;


base64_decode = (function() {
    var T = new Uint8Array(128),
	i = 0;
    for (; i < 64; ++i)
        T['ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'.charCodeAt(i)] = i;
    T[45] = 62;
    T[95] = 63;
    return function(B) {
	if (!B) return new Uint8Array(0);
	var L = B.length,
	    PH = (B[L - 2] === '=' ? 2 : (B[L - 1] === '=' ? 1 : 0)),
	    a = new Uint8Array(L * 3 / 4 - PH),
	    n = 0,
	    i = 0,
	    j = (PH > 0 ? L - 4 : L),
	    t = 0;
	while (i < j) {
	    t = (T[B.charCodeAt(i)] << 18) | (T[B.charCodeAt(i + 1)] << 12) | (T[B.charCodeAt(i + 2)] << 6) | T[B.charCodeAt(i + 3)];
	    i += 4;
	    a[n++] = (t >> 16) & 255;
	    a[n++] = (t >> 8) & 255;
	    a[n++] = t & 255
	}
	if (PH === 1) {
	    t = (T[B.charCodeAt(i)] << 10) | (T[B.charCodeAt(i + 1)] << 4) | (T[B.charCodeAt(i + 2)] >> 2);
	    a[n] = (t >> 8) & 255;
	    a[n + 1] = t & 255
	} else if (PH === 2) a[n] = ((T[B.charCodeAt(i)] << 2) | (T[B.charCodeAt(i + 1)] >> 4)) & 255;
	return a
    };
})();

if (Module["game_file"]){
    FS.createPath("/", "home", true, true);
    FS.createPath("/home", "web_user", true, true);
    FS.createPath("/home/web_user", "love", true, true);
    const fileBuffer = base64_decode(Module["game_file"]);
    FS.createDataFile('/home/web_user/love/game.love', 0, fileBuffer, true,true,true);
}
