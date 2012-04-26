#ifndef LH_PROTO_H
#define LH_PROTO_H
/*
This file was autogenerated from src/lh.c
by bin/genproto
Please do not hand edit.
*/

/** Lua helper functions. */
#include <stdlib.h>
#include <string.h>
#include <lauxlib.h>
#include "eruta.h"
#include "str.h"
#include "lh.h"
#include "fifi.h"


/* Not declared in lauxlib.h somehow, but it exists... */
void luaL_openlibs (Lua *L);

/** Lua helper functions and macros. */
Lua * lh_new();

/** Frees a lua state if it's non-NULL. */
Lua * lh_free(Lua * lua);

/** Creates lua user data in which the pointer to data will be stored. */
void ** lh_pushdata(Lua *L, const char * name, void * data);

/** Gets the object at stack index index as a pointer to data */
void * lh_todata(Lua *L,  int index);

/** Gets the object at stack index index as a pointer to data. 
Performs type checking. */
void * lh_checkdata(Lua *L, const char * name, int index);

/** Makes it easier to parse the arguments a Lua function has received.
* Works in the spirit of scanf(), but with different specifiers.
* d : double *, i: int *, l : long *, s : char **, S strdupped char **, p
* userdata pointer. Retuns amount of arguments detected.
*/
int lh_scanargs_va(Lua *L, char * format, va_list args);

/** Makes it easier to parse the arguments a Lua function has received.
* Works in the spirit of scanf(), but with different specifiers.
* d : double *, i: int *, l : long *, s : char **, D strDupped into char **,
* S : STR ** (Allegro string),  p: userdata pointer.
* Retuns amount of arguments detected.
*/
int lh_scanargs(Lua *L, char * format, ...);

/**
* Executes a file in Eruta's data/script directory.
*/
int lh_dofile(Lua *L, const char * filename);

/**
* shows an error to stderr if res is nonzero
*/
int lh_showerror_stderr(Lua * lua, int res);

/**
* Executes a file in Eruta's data/script directory, and displays any errors
* on stderr. uses the state's lua state.
*/
int lh_dofile_stderr(State * state, const char * filename);

/** Pushes values on top of the lua stack with the given argument list. Uses
* a printf-like interface so it's easier to pass around arguments.
* Returns the amount of values pushed.
* Format can contain: 'd' double, 'i' int, 'b' boolean, 's', string
* 'S' STR (allegro string) , 'p' light user data, 'f': lua_CFunction,
* '0': nil, 'v': lua_pushvalue with integer index. 
*/
int lh_push_va(Lua *L, const char * format, va_list args);

/** Pushes values on top of the lua stack with the given arguments. Uses
* printf-like interface so it's easier to pass around arguments.
* Returns the amount of values pushed.
*/
int lh_push_args(Lua *L, const char * format, ...);

/** Calls a function  after first calling lh_push_va with the
* argument list. The function should already be on the stack.
* LUA_MULTRET is used as number of results
*/
int lh_call_va(Lua * L, const char * format, va_list args);

/** Calls a function  after first calling lh_push_va with the
* arguments. The function should already be on the stack.
*/
int lh_call_args(Lua * L, const char * format, ...);

/** Calls a function  after first calling lh_push_va with the
* argument list. The function should already be on the stack.
* LUA_MULTRET is used as number of results
*/
int lh_pcall_va(Lua * L, const char * format, va_list args);

/** Calls a function  after first calling lh_push_va with the
* arguments. The function should already be on the stack.
*/
int lh_pcall_args(Lua * L, const char * format, ...);

/** Calls a named global function, as per lh_call_va */
int lh_callglobal_va(Lua * L, const char * name,
                     const char * format, va_list args);

/** Calls a named global function, as per lh_call_args */
int lh_callglobal_args(Lua * L, const char * name,
                       const char * format, ...);

/** Calls a named global function for the lua interpreter,
show errors on stderr */
int lh_callglobalstderr_va(Lua * L, const char * name,
                        const char * format, va_list args);

/** Calls a named global function for the lua interpreter,
show errors on stderr */
int lh_callglobalstderr_args(Lua * L, const char * name,
                        const char * format, ...);

/** Registers the methods and metamethods of the named type. */
int lh_register_data(Lua *L, const char * name,
                     luaL_Reg * methods, luaL_Reg * meta);

/** Like lua_rawseti, but with a string key. */
void lh_rawsets(Lua *L, char * key, int index);

/** Like lua_rawgeti but with a string key */
void lh_rawgets(Lua *L, int index, char * key);

/**Calls luaL_checktype() with a LUA_TTABLE argument. */
void lh_checktable(Lua *L, int index);

/** Utility function that calls lua_rawgeti, and then lua_tointeger and lua_pop and returns the result. In other words, it gets an integer 
value at an integer index key from the table at index on the lua stack.
**/
int lh_geti(Lua *L, int index, int key);

/** Makes a new metatable for the given type name, and prepares it so the data will be able to given methods usin gthe lh_datamethod function. If fun is not NULL, it sets the __gc value in the metatable to fun, so data of the given type will be correctly garbage collected. Must be called before calling
lh_datamethod.
*/
void lh_datainit(Lua *L, char * name, lua_CFunction fun);

/** adds a method to the given named user data type. */
void lh_datamethod(Lua *L, char * meta, char * name, lua_CFunction fun);

/** Gets a string that defines the name of the type at stack index index. */
const char * lh_typename(Lua *L, int index);

/** Walks over a table at index, calling the walker function as it goes.
* key will be at index -2, and value at index -1 
*/
void lh_tablewalk(Lua *L, int index,
                  lh_walker * walker, void * data);

/** Adds a global integer constant to the lua state */
void lh_globalint(Lua *L, const char *name, const int i);

#endif // LH_PROTO_H

