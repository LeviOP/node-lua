#include "LuaState.h"

#if defined(LUAJIT_VERSION)

#endif

#if LUA_VERSION_NUM == 501
#include "versions/51.cc"
#elif LUA_VERSION_NUM == 504
#include "versions/54.cc"
#endif

Napi::Object LuaState::Init(Napi::Env env, Napi::Object exports) {
    // This method is used to hook the accessor and method callbacks
    Napi::Function func = DefineClass(env, "LuaState", {
        InstanceMethod("call", &LuaState::Call),
        InstanceMethod("createTable", &LuaState::CreateTable),
        InstanceMethod("setField", &LuaState::SetField),
        InstanceMethod("getField", &LuaState::GetField),
        InstanceMethod("getGlobal", &LuaState::GetGlobal),
        InstanceMethod("openLibs", &LuaState::OpenLibs),
        InstanceMethod("pushString", &LuaState::PushString),
        InstanceMethod("doString", &LuaState::DoString),
        InstanceMethod("doFile", &LuaState::DoFile),
        InstanceMethod("newTable", &LuaState::NewTable),
        InstanceMethod("pushJSFunction", &LuaState::PushJSFunction),
        InstanceMethod("pushInteger", &LuaState::PushInteger),
        InstanceMethod("toInteger", &LuaState::ToInteger),
        InstanceMethod("isNumber", &LuaState::IsNumber),
        InstanceMethod("register", &LuaState::Register)
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    // Create a persistent reference to the class constructor. This will allow
    // a function called on a class prototype and a function
    // called on instance of a class to be distinguished from each other.
    *constructor = Napi::Persistent(func);
    exports.Set("LuaState", func);

    // Store the constructor as the add-on instance data. This will allow this
    // add-on to support multiple instances of itself running on multiple worker
    // threads, as well as multiple instances of itself running in different
    // contexts on the same thread.
    //
    // By default, the value set on the environment here will be destroyed when
    // the add-on is unloaded using the `delete` operator, but it is also
    // possible to supply a custom deleter.
    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}

Napi::Value LuaState::CheckStack(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), lua_checkstack(L, GetArg<int>(info, 0)));
}

void LuaState::Close(const Napi::CallbackInfo& info) {
    lua_close(L);
}

void LuaState::Concat(const Napi::CallbackInfo& info) {
    lua_concat(L, GetArg<int>(info, 0));
}

void LuaState::CreateTable(const Napi::CallbackInfo& info) {
    lua_createtable(L, GetArg<int>(info, 0), GetArg<int>(info, 1));
}

void LuaState::Error(const Napi::CallbackInfo& info) {
    lua_error(L);
}

Napi::Value LuaState::GetMetatable(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), lua_getmetatable(L, GetArg<int>(info, 0)));
}

Napi::Value LuaState::GetTop(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), lua_gettop(L));
}

void LuaState::Insert(const Napi::CallbackInfo& info) {
    lua_insert(L, GetArg<int>(info, 0));
}

Napi::Value LuaState::IsBoolean(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isboolean(L, GetArg<int>(info, 0)));
}

Napi::Value LuaState::IsJSFunction(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_iscfunction(L, GetArg<int>(info, 0)));
}

Napi::Value LuaState::IsFunction(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isfunction(L, GetArg<int>(info, 0)));
}

Napi::Value LuaState::IsLightUserdata(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_islightuserdata(L, GetArg<int>(info, 0)));
}

Napi::Value LuaState::IsNil(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isnil(L, GetArg<int>(info, 0)));
}

Napi::Value LuaState::IsNone(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isnone(L, GetArg<int>(info, 0)));
}

Napi::Value LuaState::IsNoneOrNil(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isnoneornil(L, GetArg<int>(info, 0)));
}

Napi::Value LuaState::IsNumber(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isnumber(L, GetArg<int>(info, 0)));
}

Napi::Value LuaState::IsString(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isstring(L, GetArg<int>(info, 0)));
}

Napi::Value LuaState::IsTable(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_istable(L, GetArg<int>(info, 0)));
}

Napi::Value LuaState::IsThread(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isthread(L, GetArg<int>(info, 0)));
}

Napi::Value LuaState::IsUserdata(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isuserdata(L, GetArg<int>(info, 0)));
}

void LuaState::NewTable(const Napi::CallbackInfo& info) {
    lua_newtable(L);
}

void LuaState::Next(const Napi::CallbackInfo &info) {
    lua_next(L, GetArg<int>(info, 0));
}

void LuaState::Pop(const Napi::CallbackInfo &info) {
    lua_pop(L, GetArg<int>(info, 0));
}

LuaState* get_js_state(lua_State* L) {
    lua_pushlightuserdata(L, (void*)&JSStateRegistryKey);
    lua_gettable(L, LUA_REGISTRYINDEX);
    LuaState* state = (LuaState*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    return state;
}

int callback_function(lua_State* L) {
    size_findex* fidptr = (size_findex*)lua_touserdata(L, lua_upvalueindex(1));
    LuaState* state = get_js_state(L);
    Napi::FunctionReference& func = state->functions.at(*fidptr);
    int args = func.Call({ state->Value() }).As<Napi::Number>().Int32Value();
    return args;
}

int gchook(lua_State* L) {
    size_findex fid = *(size_findex*)lua_touserdata(L, -1);
    LuaState* state = get_js_state(L);
    Napi::FunctionReference& func = state->functions.at(fid);
    func.Unref();
    state->freelist.push_back(fid);
    return 0;
}

LuaState::LuaState(const Napi::CallbackInfo& info) : Napi::ObjectWrap<LuaState>(info) {
    L = luaL_newstate();

    lua_pushlightuserdata(L, (void*)&JSStateRegistryKey);
    lua_pushlightuserdata(L, (void*)this);
    lua_settable(L, LUA_REGISTRYINDEX);

    //clua_initstate
    luaL_newmetatable(L, MT_JSFUNCTION);

    // lua_pushliteral(L, "__call");
    // lua_pushcfunction(L, &callback_function);
    // lua_settable(L,-3);

    lua_pushliteral(L, "__gc");
    lua_pushcfunction(L, &gchook);
    lua_settable(L, -3);

    lua_pop(L, 1);
}

LuaState::~LuaState() {
    if (L) {
        lua_close(L);
    }
}

void LuaState::OpenLibs(const Napi::CallbackInfo& info) {
    luaL_openlibs(L);
}

void LuaState::SetField(const Napi::CallbackInfo& info) {
    lua_setfield(L, GetArg<int>(info, 0), GetArg<const char*>(info, 1));
}

void LuaState::PushString(const Napi::CallbackInfo& info) {
    lua_pushstring(L, GetArg<const char*>(info, 0));
}

Napi::Value LuaState::ToInteger(const Napi::CallbackInfo& info) {
    int value = lua_tointeger(L, GetArg<int>(info, 0));
    return Napi::Number::New(info.Env(), value);
}

void LuaState::PushInteger(const Napi::CallbackInfo& info) {
    lua_pushinteger(L, GetArg<int>(info, 0));
}

size_findex LuaState::register_function(Napi::Function function) {
    size_findex index;
    if (freelist.size() > 0) {
        index = freelist.back();
        freelist.pop_back();
        functions.at(index) = Napi::Persistent(function);
    } else {
        index = functions.size();
        functions.push_back(Napi::Persistent(function));
    }
    return index;
}

void LuaState::push_js_closure(Napi::Function function, int n) {
    size_findex* fidptr = (size_findex*)lua_newuserdata(L, sizeof(size_findex));
    *fidptr = register_function(function);
    luaL_getmetatable(L, MT_JSFUNCTION);
    lua_setmetatable(L, -2);
    if (n > 0) lua_insert(L, -n-1);
    lua_pushcclosure(L, callback_function, 1 + n);
}

void LuaState::Register(const Napi::CallbackInfo& info) {
    push_js_closure(info[1].As<Napi::Function>(), 0);
    lua_setglobal(L, GetArg<const char*>(info, 0));
}

void LuaState::PushJSFunction(const Napi::CallbackInfo& info) {
    push_js_closure(info[0].As<Napi::Function>(), 0);
}

void LuaState::PushJSClosure(const Napi::CallbackInfo& info) {
    push_js_closure(info[0].As<Napi::Function>(), GetArg<int>(info, 1));
}

Napi::Value LuaState::DoString(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    int r;
    if (r = luaL_loadstring(L, GetArg<const char*>(info, 0)), r != 0) {
        return Napi::String::New(env, lua_tostring(L, -1));
    }
    if (r = lua_pcall(L, 0, LUA_MULTRET, 0), r != 0) {
        return Napi::String::New(env, lua_tostring(L, -1));
    }
    return env.Null();
}

Napi::Value LuaState::DoFile(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    int r;
    if (r = luaL_loadfile(L, GetArg<const char*>(info, 0)), r != 0) {
        return Napi::String::New(env, lua_tostring(L, -1));
    }
    if (r = lua_pcall(L, 0, LUA_MULTRET, 0), r != 0) {
        return Napi::String::New(env, lua_tostring(L, -1));
    }
    return env.Null();
}

Napi::Value LuaState::Call(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    lua_call(L, GetArg<int>(info, 0), GetArg<int>(info, 1));
    return env.Null();
    // return CallEx(info[0].As<Napi::Number>().Int32Value(), info[1].As<Napi::Number>().Int32Value(), true);
}

// Napi::Value LuaState::CallEx(int nargs, int nresults, bool catchError) {
//     lua_getglobal(L, NODELUA_DEFAULT_MSGHANDLER);
//     int erridx = lua_gettop(L) - nargs - 1;
//     lua_insert(L, erridx);
//     int result = lua_pcall(L, nargs, nresults, erridx);
//     lua_remove(L, erridx);
//     if (result != 0) {
//
//     }
// }

// Initialize native add-on
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    LuaState::Init(env, exports);
    return exports;
}

// Register and initialize native add-on
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
