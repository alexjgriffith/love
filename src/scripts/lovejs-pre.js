// make sure love opens the right file

if (Module["print"]){
    var lOVE2D_GAME = Module["game"] //FS.readdir("/home/web_user/love/").join(" /").match("\.love");
    let old_args = Module["arguments"];
    if (lOVE2D_GAME) {
        Module["print"]("Loading a .love file");
        Module["arguments"]=["/home/web_user/love/" + lOVE2D_GAME];
    }
    else if(Module["game_file"]){
        Module["print"]("Reading embeded .love file");
        Module["arguments"]=["/home/web_user/love/game.love"];
    }
    else {
        Module["print"]("Loading a directory file");
    Module["arguments"]=["/home/web_user/love/"];
    }
    for (arg of old_args){
        Module["arguments"].push(arg);
    }
    
    function love_receive_custom_json_event (name,details){
        const name_s = UTF8ToString(name);
        const details_s = UTF8ToString(details);
        const event = new CustomEvent(name_s, {detail: JSON.parse(details_s)});
        
        var canvas = Module["canvas"]; // - for some reason we were trying to access a function
        canvas.dispatchEvent(event);
    }
    
    function love_receive_event (name){
        const name_s = UTF8ToString(name);
        const event = new CustomEvent(name_s);
        var canvas = Module["canvas"]; // - for some reason we were trying to access a function
        canvas.dispatchEvent(event);
    }

    function love_mkdir (dir){
        FS.mkdir(dir);
    }
}
else {
    Module["print"] = (x)=>console.log(x);
}
