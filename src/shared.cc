#include "LuaState.h"

#if defined(LUAJIT_VERSION)

#endif

#if LUA_VERSION_NUM == 501
#include "versions/51.cc"
// #elif LUA_VERSION_NUM == 504
// #include "versions/54.cc"
#else
#error "Configured lua version is not supported"
#endif


Napi::Value Shared::CheckStack(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), lua_checkstack(L, GetArg<int>(info, 0)));
}

void Shared::Close(const Napi::CallbackInfo& info) {
    lua_close(L);
}

void Shared::Concat(const Napi::CallbackInfo& info) {
    lua_concat(L, GetArg<int>(info, 0));
}

void Shared::CreateTable(const Napi::CallbackInfo& info) {
    lua_createtable(L, GetArg<int>(info, 0), GetArg<int>(info, 1));
}

void Shared::Error(const Napi::CallbackInfo& info) {
    lua_error(L);
}

Napi::Value Shared::GetMetatable(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), lua_getmetatable(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::GetTop(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), lua_gettop(L));
}

void Shared::Insert(const Napi::CallbackInfo& info) {
    lua_insert(L, GetArg<int>(info, 0));
}

Napi::Value Shared::IsBoolean(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isboolean(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::IsJSFunction(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_iscfunction(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::IsFunction(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isfunction(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::IsLightUserdata(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_islightuserdata(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::IsNil(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isnil(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::IsNone(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isnone(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::IsNoneOrNil(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isnoneornil(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::IsNumber(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isnumber(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::IsString(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isstring(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::IsTable(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_istable(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::IsThread(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isthread(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::IsUserdata(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_isuserdata(L, GetArg<int>(info, 0)));
}

void Shared::NewTable(const Napi::CallbackInfo& info) {
    lua_newtable(L);
}

void Shared::Next(const Napi::CallbackInfo &info) {
    lua_next(L, GetArg<int>(info, 0));
}

void Shared::Pop(const Napi::CallbackInfo &info) {
    lua_pop(L, GetArg<int>(info, 0));
}

void Shared::PushBoolean(const Napi::CallbackInfo &info) {
    lua_pushboolean(L, GetArg<int>(info, 0));
}

size_findex Shared::register_function(Napi::Function function) {
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
    int args = func.Call({ state->Value() }).ToNumber().Int32Value();
    return args;
}


void Shared::push_js_closure(Napi::Function function, int n) {
    size_findex* fidptr = (size_findex*)lua_newuserdata(L, sizeof(size_findex));
    *fidptr = register_function(function);
    luaL_getmetatable(L, MT_JSFUNCTION);
    lua_setmetatable(L, -2);
    if (n > 0) lua_insert(L, -n-1);
    lua_pushcclosure(L, callback_function, 1 + n);
}

void Shared::PushJSClosure(const Napi::CallbackInfo& info) {
    push_js_closure(info[0].As<Napi::Function>(), GetArg<int>(info, 1));
}

void Shared::PushJSFunction(const Napi::CallbackInfo& info) {
    push_js_closure(info[0].As<Napi::Function>(), 0);
}

void Shared::PushInteger(const Napi::CallbackInfo& info) {
    lua_pushinteger(L, GetArg<int>(info, 0));
}

void Shared::PushString(const Napi::CallbackInfo& info) {
    lua_pushstring(L, GetArg<const char*>(info, 0));
}

void Shared::PushValue(const Napi::CallbackInfo& info) {
    lua_pushvalue(L, GetArg<int>(info, 0));
}

Napi::Value Shared::RawEqual(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), lua_rawequal(L, GetArg<int>(info, 0), GetArg<int>(info, 1)));
}

void Shared::RawSet(const Napi::CallbackInfo& info) {
    lua_rawset(L, GetArg<int>(info, 0));
}

void Shared::Register(const Napi::CallbackInfo& info) {
    push_js_closure(info[1].As<Napi::Function>(), 0);
    lua_setglobal(L, GetArg<const char*>(info, 0));
}

void Shared::Remove(const Napi::CallbackInfo& info) {
    lua_remove(L, GetArg<int>(info, 0));
}

void Shared::Replace(const Napi::CallbackInfo& info) {
    lua_replace(L, GetArg<int>(info, 0));
}

void Shared::Resume(const Napi::CallbackInfo& info) {
    lua_resume(L, GetArg<int>(info, 0));
}

void Shared::SetField(const Napi::CallbackInfo& info) {
    lua_setfield(L, GetArg<int>(info, 0), GetArg<const char*>(info, 1));
}

void Shared::SetGlobal(const Napi::CallbackInfo& info) {
    lua_setglobal(L, GetArg<const char*>(info, 0));
}

void Shared::SetMetatable(const Napi::CallbackInfo& info) {
    lua_setmetatable(L, GetArg<int>(info, 0));
}

void Shared::SetTable(const Napi::CallbackInfo& info) {
    lua_settable(L, GetArg<int>(info, 0));
}

void Shared::SetTop(const Napi::CallbackInfo& info) {
    lua_settop(L, GetArg<int>(info, 0));
}

Napi::Value Shared::Status(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), lua_status(L));
}

Napi::Value Shared::ToBoolean(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), lua_toboolean(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::ToJSFunction(const Napi::CallbackInfo& info) {
    if(!lua_iscfunction(L, -1)) return info.Env().Null();
    lua_getupvalue(L, GetArg<int>(info, 0), 1);
    size_findex* fidptr = (size_findex*)lua_touserdata(L, -1);
    Napi::FunctionReference& func = functions.at(*fidptr);
    return func.Value();
}

Napi::Value Shared::ToInteger(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), lua_tointeger(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::ToNumber(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), lua_tonumber(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::ToString(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), lua_tolstring(L, GetArg<int>(info, 0), NULL));
}

Napi::Value Shared::Type(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), lua_type(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::TypeName(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), lua_typename(L, GetArg<int>(info, 0)));
}

Napi::Value Shared::UpValueIndex(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), lua_upvalueindex(GetArg<int>(info, 0)));
}

Napi::Value Shared::GetUpValue(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), lua_getupvalue(L, GetArg<int>(info, 0), GetArg<int>(info, 1)));
}

Napi::Value Shared::SetUpValue(const Napi::CallbackInfo& info) {
    return Napi::String::New(info.Env(), lua_setupvalue(L, GetArg<int>(info, 0), GetArg<int>(info, 1)));
}

int gchook(lua_State* L) {
    size_findex fid = *(size_findex*)lua_touserdata(L, -1);
    Shared* state = get_js_state(L);
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

    luaL_newmetatable(L, MT_JSFUNCTION);

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

void Shared::OpenLibs(const Napi::CallbackInfo& info) {
    luaL_openlibs(L);
}

Napi::Value Shared::DoString(const Napi::CallbackInfo& info) {
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

Napi::Value Shared::DoFile(const Napi::CallbackInfo& info) {
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

Napi::Value Shared::Call(const Napi::CallbackInfo& info) {
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
    InitConstants(env, exports);
    return exports;
}

// Register and initialize native add-on
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
