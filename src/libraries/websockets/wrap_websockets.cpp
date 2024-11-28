
// EMSCRIPTEN
#include <emscripten.h>
#include <emscripten/websocket.h>

// SDL
#include <SDL_events.h>

// LUA
extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <stdio.h> // debuging only
  
int w_emscripten_websocket_push_event (char * name, char * data, int code){
  SDL_Event event;
  event.type = SDL_USEREVENT;
  event.user.data1 = name;
  event.user.data2 = data;
  event.user.code = code;
  SDL_PushEvent(&event);
  return 0;
}

char * _w_emscripten_websocket_make_name (void *userData, const char * name_string){
  char * handle_string = (char *) userData;
  size_t len = SDL_strlen(handle_string) + SDL_strlen(name_string) + 1;
  char * name = (char *) SDL_malloc(len);
  SDL_memcpy(name, handle_string, SDL_strlen(handle_string));
  SDL_memcpy(name + SDL_strlen(handle_string), name_string, SDL_strlen(name_string) + 1);
  name[len]='\0';
  return name;
}

char * _w_emscripten_websocket_make_string (const char * name_string){
  size_t len = SDL_strlen(name_string);
  char * name = (char *) SDL_malloc(len);
  SDL_memcpy(name, name_string, len);
  return name;
}
  
bool w_emscripten_websocket_onopen_callback
(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData) {
  char * name = _w_emscripten_websocket_make_name(userData, "websocketopen");
  char * message = _w_emscripten_websocket_make_string("");
  w_emscripten_websocket_push_event (name,message,websocketEvent->socket);
  // freed during push event, maybe free here instead?
  return EM_TRUE;
}

bool w_emscripten_websocket_onerror_callback
(int eventType, const EmscriptenWebSocketErrorEvent *websocketEvent, void *userData) {
  char * name = _w_emscripten_websocket_make_name(userData, "websocketerror");
  char * message = _w_emscripten_websocket_make_string("");
  w_emscripten_websocket_push_event (name,message,websocketEvent->socket);
  return EM_TRUE;
}

bool w_emscripten_websocket_onclose_callback
(int eventType, const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData) {
  const char format [64] = "{\"wasClean\":%d, \"reason\":\"%s\", \"code\":%hu}";
  char * name = _w_emscripten_websocket_make_name(userData,"websocketclose");
  char * message;
  SDL_free(userData);
  SDL_asprintf(&message,format,websocketEvent->wasClean, websocketEvent->reason, websocketEvent->code);
  w_emscripten_websocket_push_event (name , message, websocketEvent->socket);
  return EM_TRUE;
}

bool w_emscripten_websocket_onmessage_callback
(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData) {
  char * name;
  char * message;
  // printf("pre-memcopy: %s\n", (char *) websocketEvent->data);
  if(websocketEvent->isText){
    name = _w_emscripten_websocket_make_name(userData,"websocketmessage");
    message = (char *) SDL_malloc(websocketEvent->numBytes + 1);
    // does the emscripten_websocket free its own events?
    SDL_memcpy(message,websocketEvent->data,websocketEvent->numBytes + 1);
    // printf("post-memcopy: %s\n", message);
    w_emscripten_websocket_push_event (name , message, websocketEvent->socket);
  }
  return EM_TRUE;
}

int w_emscripten_websocket_get_ready_state(lua_State *L){
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *) lua_touserdata(L,1);
  unsigned short readyState;
  emscripten_websocket_get_ready_state(*ws, &readyState);
  lua_pushnumber(L,readyState);
  return 1;
}

int w_emscripten_websocket_get_buffered_amount(lua_State *L){
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *) lua_touserdata(L,1);
  size_t bufferedAmount;
  emscripten_websocket_get_buffered_amount(*ws, &bufferedAmount);
  lua_pushnumber(L, bufferedAmount);
  return 1;
}

int w_emscripten_websocket_get_url(lua_State *L){
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *) lua_touserdata(L,1);
  int length;
  emscripten_websocket_get_url_length(*ws, &length);
  char * str = (char *)  SDL_malloc (sizeof(char) * length);
  emscripten_websocket_get_url(*ws, str, length);
  lua_pushstring(L, str);
  SDL_free(str);
  return 1;
}

int w_emscripten_websocket_get_extensions(lua_State *L){
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *) lua_touserdata(L,1);
  int length;
  emscripten_websocket_get_extensions_length(*ws, &length);
  char * str = (char *) SDL_malloc (sizeof(char) * length);
  emscripten_websocket_get_extensions(*ws, str, length);
  lua_pushstring(L, str);
  SDL_free(str);
  return 1;
}

int w_emscripten_websocket_get_protocol(lua_State *L){
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *) lua_touserdata(L,1);
  int length;
  emscripten_websocket_get_protocol_length(*ws, &length);
  char * str = (char *) SDL_malloc (sizeof(char) * length);
  emscripten_websocket_get_protocol(*ws, str, length);
  lua_pushstring(L, str);
  SDL_free(str);
  return 1;
}

int w_emscripten_websocket_send_utf8_text(lua_State *L){
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *) lua_touserdata(L,1);
  const char *textData = lua_tostring(L,2);
  emscripten_websocket_send_utf8_text(*ws,textData);
  return 0;
}

int w_emscripten_websocket_send_utf8_binary(lua_State *L){
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *) lua_touserdata(L,1);
  // need to find a way to get void* out of lua
  // void *binaryData = lua_tostring(L,2);
  // emscripten_websocket_send_utf8_binary(*ws, binaryData);
  return 0;
}

int w_emscripten_websocket_send_close(lua_State *L){
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *) lua_touserdata(L,1);
  unsigned short code = lua_tonumber(L,2);
  const char *reason = lua_tostring(L,3);
  emscripten_websocket_close(*ws,code,reason);
  return 0;
}

int w_emscripten_websocket_delete(lua_State *L){
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *) lua_touserdata(L,1);
  emscripten_websocket_delete(*ws);
  return 0;
}

int w_emscripten_websocket_new(lua_State *L){
  const char * handle = luaL_checkstring(L, 1);
  const char * address = luaL_checkstring (L, 2);
  int n = lua_gettop(L);
  const char * protocols = NULL;
  if (n > 1 && lua_isstring (L, 3)){
    protocols = luaL_checkstring (L, 3);
  }
  bool main_thread;
  if ( n > 2 && lua_isboolean(L, 4)){
    main_thread = lua_toboolean(L,4);
  }
  else{
    main_thread = 0;
  }

  // _hanle is freed in the close websocket callback
  void * _handle = SDL_malloc(SDL_strlen(handle) + 1);
  SDL_memcpy(_handle,handle, SDL_strlen(handle) + 1);
  // if these attrs need to stick around we can make them into
  // another user data.
  EmscriptenWebSocketCreateAttributes ws_attrs = {
    address,
    protocols,
    main_thread
  };
  EMSCRIPTEN_WEBSOCKET_T *ws = (EMSCRIPTEN_WEBSOCKET_T *) lua_newuserdata(L, sizeof(EMSCRIPTEN_WEBSOCKET_T));
  *ws = emscripten_websocket_new(&ws_attrs);
  emscripten_websocket_set_onopen_callback(*ws, _handle, w_emscripten_websocket_onopen_callback);
  emscripten_websocket_set_onerror_callback(*ws,  _handle, w_emscripten_websocket_onerror_callback);
  emscripten_websocket_set_onclose_callback(*ws,  _handle, w_emscripten_websocket_onclose_callback);
  emscripten_websocket_set_onmessage_callback(*ws, _handle, w_emscripten_websocket_onmessage_callback);
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
  lua_pushboolean(L,is_supported);
  return 1;
}

} // extern C

//library to be registe red
static const struct luaL_Reg w_emscripten_websocket_funcs [] = {
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
  
int luaopen_websockets (lua_State *L)
{
  w_emscripten_websocket_createmeta(L);
  luaL_register(L,"websockets", w_emscripten_websocket_funcs);
  return 1;
}
