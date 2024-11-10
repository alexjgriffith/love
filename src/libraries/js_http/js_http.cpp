#include <emscripten.h>
#include <stdio.h>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

static const char js_http_javascript[] =
#include "js_http.js"
;

#define JS_MAX_QUEUED_EVENTS 100

extern "C"{
struct JS_http_event {
  char * handle;
  char * body;
  char * headers;
};

struct JS_http {
  struct JS_http_event * events;
  size_t start;
  size_t end;
  const char * root;
};
  
static int JS_call (lua_State *L) {
  const char * str = luaL_checkstring (L, 1);
  emscripten_run_script(str);
  return 0;
}

static int JS_set_lua_ptr (lua_State *L) {
  EM_ASM({__LOVE2D_set_lua_state_ptr ($0);},L);
  return 0;
}

int JS_make_http (lua_State *L) { 
  const char * root = luaL_checkstring (L, 1);
  struct JS_http *eq = (struct JS_http *) lua_newuserdata(L, sizeof(struct JS_http));
  eq->events = (struct JS_http_event *) malloc(sizeof(struct JS_http_event) * JS_MAX_QUEUED_EVENTS);
  // lua_pop(L,1);
  eq->start=0;
  eq->end=0;
  eq->root=root;
  luaL_getmetatable(L,"*HTTP");
  lua_setmetatable(L, -2);
  return 1;
}
  
EMSCRIPTEN_KEEPALIVE
void _JS_http_push_event (struct JS_http * eq, char * handle, char * body, char * headers) {
  //struct JS_http_event * e = (eq->events)[eq->end];
  struct JS_http_event * e = eq->events;
  e->handle = handle;
  e->body = body;
  e->headers = headers;
  eq->end+=1;
  if (eq->end > JS_MAX_QUEUED_EVENTS - 1){
    eq->end=0;
  }
  if (eq->start == eq->end){
    exit(1);
  }
}

EMSCRIPTEN_KEEPALIVE
void _JS_eval (lua_State * L, char * call) {
  lua_getglobal(L, "_js_eval");
  lua_pushstring(L, call);
  lua_pcall(L, 1, 0, 0);
}


int JS_http_pop_event (lua_State *L){
  int popable =  EM_ASM_INT({return __LOVE2D_response_queue_popable ();});
  if (popable){
    char * str = (char *) EM_ASM_PTR({return __LOVE2D_response_queue_pop ();});
    lua_pushstring(L,str);
  }
  else{
    lua_pushnil(L);
  }
  return 1;
}

int JS_http_request (lua_State *L){
  struct JS_http *eq = (struct JS_http *) lua_touserdata(L,1);
  const char * handle = luaL_checkstring (L, 2);
  const char * endpoint = luaL_checkstring (L, 3);
  const char * action = luaL_checkstring (L, 4);
  const char * headers = luaL_checkstring (L, 5);
  const char * data = luaL_checkstring (L, 6);
  EM_ASM({__LOVE2D_http_request($0,$1,$2,$3,$4,$5,$6,$7)}, eq, handle, eq->root, endpoint, action, headers, data);
  return 0;
}

int JS_init (lua_State *L){
  //emscripten_run_script(js_http_javascript);
  JS_set_lua_ptr(L);
  return 0;
}
}

//library to be registe red
static const struct luaL_Reg JS_http_funcs [] = {
  {"make", JS_make_http},
  {"init", JS_init},
  {"call", JS_call},
  {NULL, NULL}        // sentinel */
};

static const struct luaL_Reg JS_http_lib [] = {
  {"request", JS_http_request},
  {"pop_event", JS_http_pop_event},
  {NULL, NULL}        // sentinel */
};


static void JS_http_createmeta (lua_State *L){
  luaL_newmetatable(L, "*HTTP");
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  luaL_register(L, NULL, JS_http_lib);
}
  
int luaopen_js_http (lua_State *L)
{
  // emscripten_run_script(js_http_javascript);
  JS_http_createmeta(L);
  luaL_register(L,"js-http", JS_http_funcs);
  // JS_set_lua_ptr(L); ; has to happen during runtime and this package is preloaded
  return 1;
}

