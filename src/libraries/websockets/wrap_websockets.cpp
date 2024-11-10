#include <emscripten.h>
#include <websocket.h>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

int w_emscripten_websocket_get_ready_state(lua_State *L){
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *ws) lua_touserdata(L,1);
  unsigned short readyState;
  emscripten_websocket_get_ready_state(*ws, &readyState);
  lua_pushinteger(readyState);
  return 1;
}

int w_emscripten_websocket_get_buffered_amount(lua_State *L){
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *ws) lua_touserdata(L,1);
  size_t bufferedAmount;
  emscripten_websocket_get_buffered_amount(*ws, &bufferedAmount);
  lua_pushinteger(L, bufferedAmount);
  return 1;
}

int w_emscripten_websocket_get_url(lua_State *L){
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *ws) lua_touserdata(L,1);
  int length;
  emscripten_websocket_get_url_length(*ws, &length);
  const char * str = malloc (sizeof(char) * length);
  emscripten_websocket_get_url(*ws, str, length);
  lua_pushstring(L, str);
  free(str);
  return 1;
}

int w_emscripten_websocket_get_extensions(lua_State *L){

  return 1;
}

int w_emscripten_websocket_get_protocol(lua_State *L){

  return 1;
}


int w_emscripten_websocket_send_utf8_text(lua_State *L){

  return 1;
}

int w_emscripten_websocket_send_utf8_binary(lua_State *L){

  return 1;
}

int w_emscripten_websocket_send_close(lua_State *L){

  return 1;
}

int w_emscripten_websocket_delete(lua_State *L){

  return 1;
}

int w_emscripten_websocket_new(lua_State *L){
  const char * address = luaL_checkstring (L, 1);
  const char * protocols = luaL_checkstring (L, 2);
  bool main_thread = lua_toboolean(L, 3);
  // if these attrs need to stick around we can make them into
  // another user data.
  EmscriptenWebSocketCreateAttributes ws_attrs = {
    address,
    protocols,
    main_thread
  };
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *) lua_newuserdata(L, sizeof(EMSCRIPTEN_WEBSOCKET_T));
  *ws = emscripten_websocket_new(&ws_attrs);
  luaL_getmetatable(L,"*WEBSOCKET");
  lua_setmetatable(L, -2);
  return 1;
}

int w_emscripten_websocket_deinitialize(lua_State *L){
  emscripten_websocket_deinitialize();
  return 0;
}

int w_emscripten_websocket_is_supported(lua_State *L){
  bool is_supported = emscripten_websocket_is_supported();
  lua_pushboolean(is_supported);
  return 1;
}

//library to be registe red
static const struct luaL_Reg w_emscripten_funcs [] = {
  {"new", w_emscripten_websocket_new},
  {"deinitialize", w_emscripten_websocket_deinitialize},
  {"supported", w_emscripten_websocket_is_supported},
  {NULL, NULL}        // sentinel */
};

static const struct luaL_Reg w_emscripten_websocket_lib [] = {
  {"get_ready_state", w_emscripten_websocket_get_ready_state},
  {"get_buffered_amount", w_emscripten_websocket_get_buffered_amount},
  {"get_url", w_emscripten_websocket_get_url},
  {"get_extensions", w_emscripten_websocket_get_extensions},
  {"get_protocol", w_emscripten_websocket_get_protocol},
  {"send_utf8_text", w_emscripten_websocket_send_utf8_text},
  {"send_utf8_binary", w_emscripten_websocket_send_utf8_binary},
  {"send_close", w_emscripten_websocket_send_close},
  {"delete", w_emscripten_websocket_delete},
  {NULL, NULL}        // sentinel */
};

static void w_emscripten_websocket_createmeta (lua_State *L){
  luaL_newmetatable(L, "*WEBSOCKET");
  lua_pushvalue(L, -1);
  lua_setfield(L, -2, "__index");
  luaL_register(L, NULL, w_emscripten_websocket_lib);
}
  
int luaopen_js_http (lua_State *L)
{
  w_emscripten_websocket_createmeta(L);
  luaL_register(L,"emscripten", w_emscripten_funcs);
  return 1;
}
