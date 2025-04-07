#include "../LuaState.h"

class LuaState : public Shared, public Napi::ObjectWrap<LuaState> {
public:
    LuaState(const Napi::CallbackInfo &info);
    ~LuaState();

    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Napi::Value Equal(const Napi::CallbackInfo &info);
    Napi::Value GC(const Napi::CallbackInfo &info);
    void GetFEnv(const Napi::CallbackInfo &info);
    void GetField(const Napi::CallbackInfo &info);
    void GetGlobal(const Napi::CallbackInfo &info);
    void GetTable(const Napi::CallbackInfo &info);
    Napi::Value LessThan(const Napi::CallbackInfo &info);
    Napi::Value ObjLen(const Napi::CallbackInfo &info);
    void RawGet(const Napi::CallbackInfo &info);
    void RawGeti(const Napi::CallbackInfo &info);
    void RawSeti(const Napi::CallbackInfo &info);
    void SetFenv(const Napi::CallbackInfo &info);
};

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
        InstanceMethod("toJSFunction", &LuaState::ToJSFunction),
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

void LuaState::RawGet(const Napi::CallbackInfo& info) {
    lua_rawget(L, GetArg<int>(info, 0));
}

void LuaState::RawGeti(const Napi::CallbackInfo& info) {
    lua_rawgeti(L, GetArg<int>(info, 0), GetArg<int>(info, 1));
}

void LuaState::RawSeti(const Napi::CallbackInfo& info) {
    lua_rawseti(L, GetArg<int>(info, 0), GetArg<int>(info, 1));
}

void LuaState::SetFenv(const Napi::CallbackInfo& info) {
    lua_setfenv(L, GetArg<int>(info, 0));
}
