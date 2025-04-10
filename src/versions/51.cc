#include "../LuaState.h"

class LuaState : public Shared, public Napi::ObjectWrap<LuaState> {
public:
    LuaState(const Napi::CallbackInfo &info);
    ~LuaState();

    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    Napi::Value Equal(const Napi::CallbackInfo &info);
    Napi::Value GC(const Napi::CallbackInfo &info);
    void GetFenv(const Napi::CallbackInfo &info);
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
        InstanceMethod("checkStack", &LuaState::CheckStack),
        InstanceMethod("close", &LuaState::Close),
        InstanceMethod("concat", &LuaState::Concat),
        InstanceMethod("createTable", &LuaState::CreateTable),
        InstanceMethod("error", &LuaState::Error),
        InstanceMethod("getMetatable", &LuaState::GetMetatable),
        InstanceMethod("getTop", &LuaState::GetTop),
        InstanceMethod("insert", &LuaState::Insert),
        InstanceMethod("isBoolean", &LuaState::IsBoolean),
        InstanceMethod("isJSFunction", &LuaState::IsJSFunction),
        InstanceMethod("isFunction", &LuaState::IsFunction),
        InstanceMethod("isLightUserdata", &LuaState::IsLightUserdata),
        InstanceMethod("isNil", &LuaState::IsNil),
        InstanceMethod("isNone", &LuaState::IsNone),
        InstanceMethod("isNoneOrNil", &LuaState::IsNoneOrNil),
        InstanceMethod("isNumber", &LuaState::IsNumber),
        InstanceMethod("isString", &LuaState::IsString),
        InstanceMethod("isTable", &LuaState::IsTable),
        InstanceMethod("isThread", &LuaState::IsThread),
        InstanceMethod("isUserdata", &LuaState::IsUserdata),
        InstanceMethod("newTable", &LuaState::NewTable),
        InstanceMethod("next", &LuaState::Next),
        InstanceMethod("pop", &LuaState::Pop),
        InstanceMethod("pushBoolean", &LuaState::PushBoolean),
        InstanceMethod("pushJSClosure", &LuaState::PushJSClosure),
        InstanceMethod("pushJSFunction", &LuaState::PushJSFunction),
        InstanceMethod("pushInteger", &LuaState::PushInteger),
        InstanceMethod("pushString", &LuaState::PushString),
        InstanceMethod("pushValue", &LuaState::PushValue),
        InstanceMethod("rawEqual", &LuaState::RawEqual),
        InstanceMethod("rawSet", &LuaState::RawSet),
        InstanceMethod("register", &LuaState::Register),
        InstanceMethod("remove", &LuaState::Remove),
        InstanceMethod("replace", &LuaState::Replace),
        InstanceMethod("resume", &LuaState::Resume),
        InstanceMethod("setField", &LuaState::SetField),
        InstanceMethod("setGlobal", &LuaState::SetGlobal),
        InstanceMethod("setMetatable", &LuaState::SetMetatable),
        InstanceMethod("setTable", &LuaState::SetTable),
        InstanceMethod("setTop", &LuaState::SetTop),
        InstanceMethod("status", &LuaState::Status),
        InstanceMethod("toBoolean", &LuaState::ToBoolean),
        InstanceMethod("toJSFunction", &LuaState::ToJSFunction),
        InstanceMethod("toInteger", &LuaState::ToInteger),
        InstanceMethod("toNumber", &LuaState::ToNumber),
        InstanceMethod("toString", &LuaState::ToString),
        InstanceMethod("type", &LuaState::Type),
        InstanceMethod("typeName", &LuaState::TypeName),
        InstanceMethod("upValueIndex", &LuaState::UpValueIndex), // Not really a method, but I don't know where else to put it
        InstanceMethod("getUpValue", &LuaState::GetUpValue),
        InstanceMethod("setUpValue", &LuaState::SetUpValue),

        InstanceMethod("equal", &LuaState::Equal),
        InstanceMethod("gc", &LuaState::GC),
        InstanceMethod("getFenv", &LuaState::GetFenv),
        InstanceMethod("getField", &LuaState::GetField),
        InstanceMethod("getGlobal", &LuaState::GetGlobal),
        InstanceMethod("getTable", &LuaState::GetTable),
        InstanceMethod("lessThan", &LuaState::LessThan),
        InstanceMethod("objLen", &LuaState::ObjLen),
        InstanceMethod("rawGet", &LuaState::RawGet),
        InstanceMethod("rawGeti", &LuaState::RawGeti),
        InstanceMethod("rawSeti", &LuaState::RawSeti),
        InstanceMethod("setFenv", &LuaState::SetFenv),

        InstanceMethod("openLibs", &LuaState::OpenLibs),
        InstanceMethod("doFile", &LuaState::DoFile),
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

void InitConstants(Napi::Env env, Napi::Object exports) {
    Napi::Object luaConstants = Napi::Object::New(env);
    luaConstants.Set("VERSION", Napi::String::New(env, LUA_VERSION));
    luaConstants.Set("VERSION_NUM", Napi::Number::New(env, LUA_VERSION_NUM));
    luaConstants.Set("COPYRIGHT", Napi::String::New(env, LUA_COPYRIGHT));
    luaConstants.Set("AUTHORS", Napi::String::New(env, LUA_AUTHORS));

    luaConstants.Set("MULTRET", Napi::Number::New(env, LUA_MULTRET));

    luaConstants.Set("REGISTRYINDEX", Napi::Number::New(env, LUA_REGISTRYINDEX));
    luaConstants.Set("ENVIRONINDEX", Napi::Number::New(env, LUA_ENVIRONINDEX));
    luaConstants.Set("GLOBALSINDEX", Napi::Number::New(env, LUA_GLOBALSINDEX));

    luaConstants.Set("OK", Napi::Number::New(env, LUA_OK));
    luaConstants.Set("YIELD", Napi::Number::New(env, LUA_YIELD));
    luaConstants.Set("ERRRUN", Napi::Number::New(env, LUA_ERRRUN));
    luaConstants.Set("ERRSYNTAX", Napi::Number::New(env, LUA_ERRSYNTAX));
    luaConstants.Set("ERRMEM", Napi::Number::New(env, LUA_ERRMEM));
    luaConstants.Set("ERRERR", Napi::Number::New(env, LUA_ERRERR));

    luaConstants.Set("TNONE", Napi::Number::New(env, LUA_TNONE));
    luaConstants.Set("TNIL", Napi::Number::New(env, LUA_TNIL));
    luaConstants.Set("TBOOLEAN", Napi::Number::New(env, LUA_TBOOLEAN));
    luaConstants.Set("TLIGHTUSERDATA", Napi::Number::New(env, LUA_TLIGHTUSERDATA));
    luaConstants.Set("TNUMBER", Napi::Number::New(env, LUA_TNUMBER));
    luaConstants.Set("TNUMBER", Napi::Number::New(env, LUA_TNUMBER));
    luaConstants.Set("TSTRING", Napi::Number::New(env, LUA_TSTRING));
    luaConstants.Set("TTABLE", Napi::Number::New(env, LUA_TTABLE));
    luaConstants.Set("TFUNCTION", Napi::Number::New(env, LUA_TFUNCTION));
    luaConstants.Set("TUSERDATA", Napi::Number::New(env, LUA_TUSERDATA));
    luaConstants.Set("TTHREAD", Napi::Number::New(env, LUA_TTHREAD));

    luaConstants.Set("MINSTACK", Napi::Number::New(env, LUA_MINSTACK));

    luaConstants.Set("GCSTOP", Napi::Number::New(env, LUA_GCSTOP));
    luaConstants.Set("GCRESTART", Napi::Number::New(env, LUA_GCRESTART));
    luaConstants.Set("GCCOLLECT", Napi::Number::New(env, LUA_GCCOLLECT));
    luaConstants.Set("GCCOUNT", Napi::Number::New(env, LUA_GCCOUNT));
    luaConstants.Set("GCCOUNTB", Napi::Number::New(env, LUA_GCCOUNTB));
    luaConstants.Set("GCSTEP", Napi::Number::New(env, LUA_GCSTEP));
    luaConstants.Set("GCSETPAUSE", Napi::Number::New(env, LUA_GCSETPAUSE));
    luaConstants.Set("GCSETSTEPMUL", Napi::Number::New(env, LUA_GCSETSTEPMUL));
    luaConstants.Set("GCISRUNNING", Napi::Number::New(env, LUA_GCISRUNNING));

    exports.Set("LUA", luaConstants);
}

Napi::Value LuaState::Equal(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), lua_equal(L, GetArg<int>(info, 0), GetArg<int>(info, 1)));
}

Napi::Value LuaState::GC(const Napi::CallbackInfo& info) {
    return Napi::Number::New(info.Env(), lua_gc(L, GetArg<int>(info, 0), GetArg<int>(info, 1)));
}

void LuaState::GetFenv(const Napi::CallbackInfo& info) {
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
