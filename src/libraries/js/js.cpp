#include <emscripten.h>
#include <stdio.h>

// SDL
#include <SDL_events.h>


extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// static const char js_http_javascript[] =
// #include "js.js"
// ;

// static const char js_lua[] =
// #include "js.lua"
// ;

extern "C"{
  
static int JS_call (lua_State *L) {
  const char * str = luaL_checkstring (L, 1);
  emscripten_run_script(str);
  return 0;
}

static int JS_lua_send_custom_json_event (lua_State *L) {
  const char * eventName = luaL_checkstring (L, 1);
  const char * jsonData = luaL_checkstring (L, 2);
  EM_ASM({love_receive_custom_json_event($0,$1);}, eventName, jsonData);
  return 0;
}

static int JS_lua_send_event (lua_State *L) {
  const char * eventName = luaL_checkstring (L, 1);
  EM_ASM({love_receive_event($0);}, eventName);
  return 0;
}
  
EMSCRIPTEN_KEEPALIVE
int JS_send_event (char * handle, char * data, int code){
  SDL_Event event;
  event.type = SDL_USEREVENT;
  event.user.data1 = (char *) malloc(SDL_strlen(handle) );
  SDL_memcpy(event.user.data1,handle,SDL_strlen(handle));
  ((char *) event.user.data1)[SDL_strlen(handle)] = '\0';
  event.user.data2 = (char *) malloc(SDL_strlen(data) );
  SDL_memcpy(event.user.data2,data,SDL_strlen(data));
  ((char *) event.user.data2)[SDL_strlen(data)] = '\0';
  event.user.code = code;
  SDL_PushEvent(&event);
  // handle and data get freed when this call ends
  return 0;
}

int example_event (lua_State *L){
  SDL_Event event;
  event.type = SDL_USEREVENT;
  event.user.data1 = (char *) malloc(sizeof(char) * 16);
  event.user.data2 = (char *) malloc(sizeof(char) * 16);
  ((char *) event.user.data1)[0]='t';
  ((char *) event.user.data1)[1]='e';
  ((char *) event.user.data1)[2]='s';
  ((char *) event.user.data1)[3]='t';
  ((char *) event.user.data1)[4]='\0';
  ((char *) event.user.data2)[0]='d';
  ((char *) event.user.data2)[1]='a';
  ((char *) event.user.data2)[2]='t';
  ((char *) event.user.data2)[3]='a';
  ((char *) event.user.data2)[4]='\0';
  event.user.code = 0;
  SDL_PushEvent(&event);
  return 0;
}
  
  
} // extern C

//library to be registe red
static const struct luaL_Reg JS_funcs [] = {
  {"call", JS_call},
  {"send-custom-json-event", JS_lua_send_custom_json_event},
  {"send-event", JS_lua_send_event},
  {"example-event", example_event},
  {NULL, NULL}        // sentinel */
};


int luaopen_js (lua_State *L)
{
  luaL_register(L,"js", JS_funcs);
  // if (luaL_loadbuffer(L, (const char *)js_lua, sizeof(js_lua), "=[love \"js.lua\"]") == 0)
  //   lua_call(L, 0, 0);
  // else
  //   lua_error(L);
  return 1;
}

