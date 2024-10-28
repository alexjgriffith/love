//R"JS_String"//(
var __LOVE2D_lua_state_ptr = null;

function __LOVE2D_set_lua_state_ptr (ptr){
    __LOVE2D_lua_state_ptr = ptr;
}

function __LOVE2D_eval (str){
    __JS_eval(__LOVE2D_lua_state_ptr, stringToNewUTF8(str));
}

function __LOVE2D_http_request (ptr, handle, root, endpoint, action, headers, data) {
    console.log({ptr: ptr,
                 path: UTF8ToString(root) + UTF8ToString(endpoint),
                 handle: UTF8ToString(handle),
                 endpoint: UTF8ToString(endpoint),
                 action : UTF8ToString(action),
                 headers: UTF8ToString(headers),
                 data: UTF8ToString(data)})
    const path = UTF8ToString(root) + UTF8ToString(endpoint);
    if (ptr == null){
        return 1;
    }
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            // this creates a memory leak?
            __JS_http_push_event(ptr,
                                 stringToNewUTF8(UTF8ToString(handle)),
                                 stringToNewUTF8(xhttp.responseText),
                                 stringToNewUTF8("{}"));
        }
    };    
    if (headers != null) {
        Object.keys(JSON.parse(UTF8ToString(headers))).forEach(function(key, index) {
        headers[key] *= 2;
        });
    }    
    if (action != null){
        xhttp.open(UTF8ToString(action), path, true);
    }
    else {
        xhttp.open("GET", path, true);
    }
    
    if (data != null){
        xhttp.send(UTF8ToString(data));
    }
    else {
        xhttp.send();
    }
    return 0;
}
//)JS_String"//"
