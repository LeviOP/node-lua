import { Method } from "./generate-definitions.js";

const NAMEMAP = new Map<string, string>([
    ["lua_atpanic", "atPanic"],
    ["lua_call", "call"],
    ["lua_checkstack", "checkStack"],
    ["lua_close", "close"],
    ["lua_concat", "concat"],
    ["lua_cpcall", "cpCall"],
    ["lua_createtable", "createTable"],
    ["lua_dump", "dump"],
    ["lua_equal", "equal"],
    ["lua_error", "error"],
    ["lua_gc", "gc"],
    ["lua_getallocf", "getAllocF"],
    ["lua_getfenv", "getFEnv"],
    ["lua_getfield", "getField"],
    ["lua_getglobal", "getGlobal"],
    ["lua_getmetatable", "getMetatable"],
    ["lua_gettable", "getTable"],
    ["lua_gettop", "getTop"],
    ["lua_insert", "insert"],
    ["lua_isboolean", "isBoolean"],
    ["lua_iscfunction", "isCFunction"],
    ["lua_isfunction", "isFunction"],
    ["lua_islightuserdata", "isLightUserdata"],
    ["lua_isnil", "isNil"],
    ["lua_isnone", "isNone"],
    ["lua_isnoneornil", "isNoneOrNil"],
    ["lua_isnumber", "isNumber"],
    ["lua_isstring", "isString"],
    ["lua_istable", "isTable"],
    ["lua_isthread", "isThread"],
    ["lua_isuserdata", "isUserdata"],
    ["lua_lessthan", "lessThan"],
    ["lua_load", "load"],
    ["lua_newtable", "newTable"],
    ["lua_newthread", "newThread"],
    ["lua_newuserdata", "newUserdata"],
    ["lua_next", "next"],
    ["lua_objlen", "objLen"],
    ["lua_pcall", "pCall"],
    ["lua_pop", "pop"],
    ["lua_pushboolean", "pushBoolean"],
    ["lua_pushcclosure", "pushCClosure"],
    ["lua_pushcfunction", "pushCFunction"],
    ["lua_pushinteger", "pushInteger"],
    ["lua_pushlightuserdata", "pushLightUserdata"],
    ["lua_pushliteral", "pushLiteral"],
    ["lua_pushlstring", "pushLString"],
    ["lua_pushnil", "pushNil"],
    ["lua_pushnumber", "pushNumber"],
    ["lua_pushstring", "pushString"],
    ["lua_pushthread", "pushThread"],
    ["lua_pushvalue", "pushValue"],
    ["lua_pushvfstring", "pushVFString"],
    ["lua_rawequal", "rawEqual"],
    ["lua_rawget", "rawGet"],
    ["lua_rawgeti", "rawGetI"],
    ["lua_rawset", "rawSet"],
    ["lua_rawseti", "rawSetI"],
    ["lua_register", "register"],
    ["lua_remove", "remove"],
    ["lua_replace", "replace"],
    ["lua_resume", "resume"],
    ["lua_setallocf", "setAllocF"],
    ["lua_setfenv", "setFEnv"],
    ["lua_setfield", "setField"],
    ["lua_setglobal", "setGlobal"],
    ["lua_setmetatable", "setMetatable"],
    ["lua_settable", "setTable"],
    ["lua_settop", "setTop"],
    ["lua_status", "status"],
    ["lua_toboolean", "toBoolean"],
    ["lua_tocfunction", "toCFunction"],
    ["lua_tointeger", "toInteger"],
    ["lua_tolstring", "toLString"],
    ["lua_tonumber", "toNumber"],
    ["lua_topointer", "toPointer"],
    ["lua_tostring", "toString"],
    ["lua_tothread", "toThread"],
    ["lua_touserdata", "toUserdata"],
    ["lua_type", "type"],
    ["lua_typename", "typeName"],
    ["lua_xmove", "xMove"],
    ["lua_yield", "yield"],
    ["lua_atpanic", "atPanic"],
    ["lua_call", "call"],
    ["lua_checkstack", "checkStack"],
    ["lua_close", "close"],
    ["lua_concat", "concat"],
    ["lua_gethookcount", "getHookCount"],
    ["lua_gethookmask", "getHookMask"],
    ["lua_getstack", "getStack"],
    ["lua_getupvalue", "getUpvalue"],
    ["lua_setupvalue", "setUpvalue"],
    ["luaL_addchar", "auxAddChar"],
    ["luaL_addlstring", "auxAddLString"],
    ["luaL_addsize", "auxAddSize"],
    ["luaL_addstring", "auxAddString"],
    ["luaL_addvalue", "auxAddValue"],
    ["luaL_argcheck", "auxArgCheck"],
    ["luaL_argerror", "auxArgError"],
    ["luaL_buffinit", "auxBuffInit"],
    ["luaL_callmeta", "auxCallMeta"],
    ["luaL_checkany", "auxCheckAny"],
    ["luaL_checkint", "auxCheckInt"],
    ["luaL_checkinteger", "auxCheckInteger"],
    ["luaL_checklong", "auxCheckLong"],
    ["luaL_checklstring", "auxCheckLString"],
    ["luaL_checknumber", "auxCheckNumber"],
    ["luaL_checkoption", "auxCheckOption"],
    ["luaL_checkstack", "auxCheckStack"],
    ["luaL_checkstring", "auxCheckString"],
    ["luaL_checktype", "auxCheckType"],
    ["luaL_checkudata", "auxCheckUData"],
    ["luaL_dofile", "auxDoFile"],
    ["luaL_dostring", "auxDoString"],
    ["luaL_getmetafield", "auxGetMetafield"],
    ["luaL_getmetatable", "auxGetMetatable"],
    ["luaL_gsub", "auxGSub"],
    ["luaL_loadbuffer", "auxLoadBuffer"],
    ["luaL_loadfile", "auxLoadFile"],
    ["luaL_loadstring", "auxLoadString"],
    ["luaL_newmetatable", "auxNewMetatable"],
    ["luaL_openlibs", "auxOpenLibs"],
    ["luaL_optint", "auxOptInt"],
    ["luaL_optinteger", "auxOptInteger"],
    ["luaL_optlong", "auxOptLong"],
    ["luaL_optlstring", "auxOptLString"],
    ["luaL_optnumber", "auxOptNumber"],
    ["luaL_optstring", "auxOptString"],
    ["luaL_prepbuffer", "auxPrepBuffer"],
    ["luaL_pushresult", "auxPushResult"],
    ["luaL_ref", "auxRef"],
    ["luaL_register", "auxRegister"],
    ["luaL_typename", "auxTypeName"],
    ["luaL_typerror", "auxTypeError"],
    ["luaL_unref", "auxUnref"],
    ["luaL_where", "auxWhere"]
]);

const IGNORELIST = [
    "lua_cpcall",
    "lua_close",
    "lua_dump",
    "lua_error",
    "lua_getallocf",
    "lua_pushlightuserdata",
    "lua_setallocf",
    "lua_load",
    "lua_newuserdata",
    "lua_tolstring",
    "lua_pushlstring",
    "lua_pushfstring",
    "lua_pushvfstring",
    "luaL_error",
    "lua_newstate",
    "lua_newthread",
    "lua_tothread",
    "lua_xmove",
    "lua_tocfunction",
    "lua_atpanic",
    "lua_pushliteral",
    "lua_yield",
    "lua_pushcfunction",
    "lua_pushcclosure",
    "lua_iscfunction",
    "lua_tocfunction",
    "lua_register",
    "lua_gethook",
    "lua_sethook",
    "lua_getinfo",
    "lua_getlocal",
    "lua_setlocal",
    "luaL_newstate",
    "lua_getstack",
    "luaL_addchar",
    "luaL_addlstring",
    "luaL_addsize",
    "luaL_addstring",
    "luaL_addvalue",
    "luaL_buffinit",
    "luaL_prepbuffer",
    "luaL_pushresult",
    "luaL_checklong",
    "luaL_checkoption",
    "luaL_optlong",
    "luaL_register"
];

const HARDCODED: Method[] = [
    {
        "fragment": "lua_pushcfunction",
        "indicator": "[-0, +1, m]",
        "cFunction": {
            "declaration": {
                "name": "lua_pushcfunction",
                "type": "void",
                "isPointer": false
            },
            "parameters": [
                {
                    "type": "lua_State",
                    "name": "L",
                    "isPointer": true
                },
                {
                    "type": "lua_CFunction",
                    "name": "f",
                    "isPointer": false
                }
            ]
        },
        "jsName": "pushJSFunction",
        "doc": "Pushes a JS function onto the stack. This function receives a JS function and pushes onto the stack a Lua value of type `function` that, when called, invokes the corresponding JS function.\n\nAny function to be registered in Lua must follow the correct protocol to receive its parameters and return its results (see [`lua_CFunction`](https://www.lua.org/manual/5.1/manual.html#lua_CFunction)).\n\n`lua_pushcfunction` is defined as a macro:\n```c\n     #define lua_pushcfunction(L,f)  lua_pushcclosure(L,f,0)\n```"
    },
    {
        "fragment": "lua_pushcclosure",
        "indicator": "[-n, +1, m]",
        "cFunction": {
            "declaration": {
                "name": "lua_pushcclosure",
                "type": "void",
                "isPointer": false
            },
            "parameters": [
                {
                    "type": "lua_State",
                    "name": "L",
                    "isPointer": true
                },
                {
                    "type": "lua_CFunction",
                    "name": "fn",
                    "isPointer": false
                },
                {
                    "type": "int",
                    "name": "n",
                    "isPointer": false
                }
            ]
        },
        "jsName": "pushJSClosure",
        "doc": "Pushes a new JS closure onto the stack.\n\nWhen a JS function is created, it is possible to associate some values with it, thus creating a JS closure (see [§3.4](https://www.lua.org/manual/5.1/manual.html#3.4)); these values are then accessible to the function whenever it is called. To associate values with a JS function, first these values should be pushed onto the stack (when there are multiple values, the first value is pushed first). Then [`lua_pushcclosure`](https://www.lua.org/manual/5.1/manual.html#lua_pushcclosure) is called to create and push the JS function onto the stack, with the argument `n` telling how many values should be associated with the function. [`lua_pushcclosure`](https://www.lua.org/manual/5.1/manual.html#lua_pushcclosure) also pops these values from the stack.\n\nThe maximum value for `n` is 255."
    },
    {
        "fragment": "lua_iscfunction",
        "indicator": "[-0, +0, -]",
        "cFunction": {
            "declaration": {
                "name": "lua_iscfunction",
                "type": "int",
                "isPointer": false
            },
            "parameters": [
                {
                    "type": "lua_State",
                    "name": "L",
                    "isPointer": true
                },
                {
                    "type": "int",
                    "name": "index",
                    "isPointer": false
                }
            ]
        },
        "jsName": "isJSFunction",
        "doc": "Returns 1 if the value at the given acceptable index is a JS function, and 0 otherwise."
    },
    {
        "fragment": "lua_tocfunction",
        "indicator": "[-0, +0, -]",
        "cFunction": {
            "declaration": {
                "name": "lua_iscfunction",
                "type": "int",
                "isPointer": false
            },
            "parameters": [
                {
                    "type": "lua_State",
                    "name": "L",
                    "isPointer": true
                },
                {
                    "type": "int",
                    "name": "index",
                    "isPointer": false
                }
            ]
        },
        "jsName": "toJSFunction",
        "doc": "Converts a value at the given acceptable index to a JS function. That value must be a JS function; otherwise, returns `NULL`."
    },
    {
        "fragment": "lua_register",
        "indicator": "[-0, +0, -]",
        "cFunction": {
            "declaration": {
                "name": "lua_register",
                "type": "int",
                "isPointer": false
            },
            "parameters": [
                {
                    "type": "lua_State",
                    "name": "L",
                    "isPointer": true
                },
                {
                    "type": "char",
                    "name": "name",
                    "isPointer": true
                },
                {
                    "type": "lua_CFunction",
                    "name": "f",
                    "isPointer": false
                }
            ]
        },
        "jsName": "register",
        "doc": "Sets the JS function `f` as the new value of global `name`. It is defined as a macro:\n```c\n    #define lua_register(L,n,f) \\\n           (lua_pushcfunction(L, f), lua_setglobal(L, n))\n```"
    }
];

export {
    NAMEMAP,
    IGNORELIST,
    HARDCODED
};
