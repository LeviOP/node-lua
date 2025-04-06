#include "../LuaState.h"

Napi::Value LuaState::Equal(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), lua_equal(L, GetArg<int>(info, 0), GetArg<int>(info, 1)));
}

Napi::Value LuaState::GC(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), lua_gc(L, GetArg<int>(info, 0), GetArg<int>(info, 1)));
}

void LuaState::GetFEnv(const Napi::CallbackInfo& info) {
    lua_getfenv(L, GetArg<int>(info, 0));
}

void LuaState::GetField(const Napi::CallbackInfo& info) {
    lua_getfield(L, GetArg<int>(info, 0), GetArg<const char*>(info, 1));
}

void LuaState::GetGlobal(const Napi::CallbackInfo& info) {
    lua_getglobal(L, GetArg<const char*>(info, 0));
}

void LuaState::GetTable(const Napi::CallbackInfo& info) {
    lua_gettable(L, GetArg<int>(info, 0));
}

Napi::Value LuaState::LessThan(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), lua_lessthan(L, GetArg<int>(info, 0), GetArg<int>(info, 1)));
}

Napi::Value LuaState::ObjLen(const Napi::CallbackInfo &info) {
    return Napi::Number::New(info.Env(), lua_objlen(L, GetArg<int>(info, 0)));
}
