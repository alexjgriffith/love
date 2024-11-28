// make sure love opens the right file
var lOVE2D_GAME = null;
if (lOVE2D_GAME)
    Module["arguments"]=["/home/web_user/love/" + lOVE2D_GAME];
else
    Module["arguments"]=["/home/web_user/love/"];

// love_send_event("eval", "(let [js (require :js)] (js.send-custom-json-event :echo \"{}\"))",0)

function love_receive_custom_json_event (name,details){
    const name_s = UTF8ToString(name);
    const details_s = UTF8ToString(details);
    const event = new CustomEvent(name_s, {detail: JSON.parse(details_s)});
    var canvas = document.getElementById("canvas");
    canvas.dispatchEvent(event);
}

function love_receive_event (name){
    const name_s = UTF8ToString(name);
    const event = new CustomEvent(name_s);
    var canvas = document.getElementById("canvas");
    canvas.dispatchEvent(event);
}
