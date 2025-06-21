#include <cassert>
#include <lua.hpp>
#include <napi.h>

#define MT_JSFUNCTION "nodelua.jsfunction"
static const char JSStateRegistryKey = 'k';

typedef std::vector<Napi::FunctionReference>::size_type size_findex;

template <typename T> T GetArg(const Napi::CallbackInfo &info, size_t index);

template <>
inline int GetArg<int>(const Napi::CallbackInfo &info, size_t index) {
    int64_t raw = info[index].As<Napi::Number>().Int64Value();
    assert(raw >= std::numeric_limits<int>::min() &&
           raw <= std::numeric_limits<int>::max());
    return static_cast<int>(raw);
}

template <>
inline long GetArg<long>(const Napi::CallbackInfo &info, size_t index) {
    int64_t raw = info[index].As<Napi::Number>().Int64Value();
    assert(raw >= std::numeric_limits<long>::min() &&
           raw <= std::numeric_limits<long>::max());
    return static_cast<long>(raw);
}

template <>
inline double GetArg<double>(const Napi::CallbackInfo &info, size_t index) {
    return info[index].As<Napi::Number>().DoubleValue();
}

template <>
inline const char *GetArg<const char *>(const Napi::CallbackInfo &info, size_t index) {
    static thread_local std::string temp;
    temp = info[index].As<Napi::String>().Utf8Value();
    return temp.c_str();
}

class LuaState : public Napi::ObjectWrap<LuaState>  {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    LuaState(const Napi::CallbackInfo& info);
    ~LuaState();
    std::vector<Napi::FunctionReference> functions;
    std::vector<size_findex> freelist;
    lua_State *L;

    void push_js_closure(Napi::Function function, int n);
    size_findex register_function(Napi::Function function);

    void PushJSClosure(const Napi::CallbackInfo& info);
    void PushJSFunction(const Napi::CallbackInfo& info);
    Napi::Value IsJSFunction(const Napi::CallbackInfo& info);
    Napi::Value ToJSFunction(const Napi::CallbackInfo& info);
    void Register(const Napi::CallbackInfo& info);

    #include "methods.h"
};

LuaState* get_js_state(lua_State* L) {
    lua_pushlightuserdata(L, (void*)&JSStateRegistryKey);
    lua_gettable(L, LUA_REGISTRYINDEX);
    LuaState* state = (LuaState*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    return state;
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

int callback_function(lua_State* L) {
    size_findex* fidptr = (size_findex*)lua_touserdata(L, lua_upvalueindex(1));
    LuaState* state = get_js_state(L);
    Napi::FunctionReference& func = state->functions.at(*fidptr);
    int args = func.Call({ state->Value() }).ToNumber().Int32Value();
    return args;
}

void LuaState::push_js_closure(Napi::Function function, int n) {
    size_findex* fidptr = (size_findex*)lua_newuserdata(L, sizeof(size_findex));
    *fidptr = register_function(function);
    luaL_getmetatable(L, MT_JSFUNCTION);
    lua_setmetatable(L, -2);
    if (n > 0) lua_insert(L, -n-1);
    lua_pushcclosure(L, callback_function, 1 + n);
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

void LuaState::PushJSClosure(const Napi::CallbackInfo& info) {
    push_js_closure(info[0].As<Napi::Function>(), GetArg<int>(info, 1));
}

void LuaState::PushJSFunction(const Napi::CallbackInfo& info) {
    push_js_closure(info[0].As<Napi::Function>(), 0);
}

Napi::Value LuaState::IsJSFunction(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), lua_iscfunction(L, GetArg<int>(info, 0)));
}

Napi::Value LuaState::ToJSFunction(const Napi::CallbackInfo& info) {
    if(!lua_iscfunction(L, -1)) return info.Env().Null();
    lua_getupvalue(L, GetArg<int>(info, 0), 1);
    size_findex* fidptr = (size_findex*)lua_touserdata(L, -1);
    Napi::FunctionReference& func = functions.at(*fidptr);
    return func.Value();
}

void LuaState::Register(const Napi::CallbackInfo& info) {
    push_js_closure(info[1].As<Napi::Function>(), 0);
    lua_setglobal(L, GetArg<const char*>(info, 0));
}

Napi::Object LuaState::Init(Napi::Env env, Napi::Object exports) {
    // This method is used to hook the accessor and method callbacks
    Napi::Function func = DefineClass(env, "LuaState", {
        InstanceMethod("pushJSClosure", &LuaState::PushJSClosure),
        InstanceMethod("pushJSFunction", &LuaState::PushJSFunction),
        InstanceMethod("register", &LuaState::Register),
        #include "properties.h"
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(func);
    exports.Set("LuaState", func);

    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    LuaState::Init(env, exports);
    // InitConstants(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
