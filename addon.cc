#include <cassert>
#include <napi.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#define NODELUA_DEFAULT_MSGHANDLER "nodelua_default_msghandler"
#define MT_JSFUNCTION "nodelua.jsfunction"

static const char JSStateRegistryKey = 'k';

typedef std::vector<Napi::FunctionReference>::size_type size_findex;

template<typename T>
T GetArg(const Napi::CallbackInfo& info, size_t index);

template<>
inline int GetArg<int>(const Napi::CallbackInfo& info, size_t index) {
    int64_t raw = info[index].As<Napi::Number>().Int64Value();
    assert(raw >= std::numeric_limits<int>::min() && raw <= std::numeric_limits<int>::max());
    return static_cast<int>(raw);
}

template<>
inline const char* GetArg<const char*>(const Napi::CallbackInfo& info, size_t index) {
    static thread_local std::string temp;
    temp = info[index].As<Napi::String>().Utf8Value();
    return temp.c_str();
}

class LuaState : public Napi::ObjectWrap<LuaState> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    LuaState(const Napi::CallbackInfo& info);
    ~LuaState();
    std::vector<Napi::FunctionReference> functions;
    std::vector<size_findex> freelist;
    lua_State *L;

private:
    void push_js_closure(Napi::Function function, int n);
    size_findex register_function(Napi::Function function);
    void callback();

    Napi::Value CallEx(int nargs, int nresults, bool catchError);

    Napi::Value Call(const Napi::CallbackInfo& info);
    Napi::Value CheckStack(const Napi::CallbackInfo& info);
    void Close(const Napi::CallbackInfo& info);
    void Concat(const Napi::CallbackInfo& info);
    // void CPCall(const Napi::CallbackInfo& info);
    void CreateTable(const Napi::CallbackInfo& info);
    // void Dump(const Napi::CallbackInfo& info);
    void Equal(const Napi::CallbackInfo& info);
    void Error(const Napi::CallbackInfo& info);
    void GC(const Napi::CallbackInfo& info);
    void GetAllocF(const Napi::CallbackInfo& info);
    void GetFEnv(const Napi::CallbackInfo& info);
    void GetField(const Napi::CallbackInfo& info);
    void GetGlobal(const Napi::CallbackInfo& info);
    void GetMetatable(const Napi::CallbackInfo& info);
    void GetTable(const Napi::CallbackInfo& info);
    void GetTop(const Napi::CallbackInfo& info);
    void Insert(const Napi::CallbackInfo& info);
    void IsBoolean(const Napi::CallbackInfo& info);
    void IsJSFunction(const Napi::CallbackInfo& info);
    void IsFunction(const Napi::CallbackInfo& info);
    void IsLightUserData(const Napi::CallbackInfo& info);
    void IsNil(const Napi::CallbackInfo& info);
    void IsNone(const Napi::CallbackInfo& info);
    void IsNoneOrNil(const Napi::CallbackInfo& info);
    Napi::Value IsNumber(const Napi::CallbackInfo& info);
    void IsString(const Napi::CallbackInfo& info);
    void IsTable(const Napi::CallbackInfo& info);
    void IsThread(const Napi::CallbackInfo& info);
    void IsUserData(const Napi::CallbackInfo& info);
    void LessThan(const Napi::CallbackInfo& info);
    void Load(const Napi::CallbackInfo& info);
    void NewTable(const Napi::CallbackInfo& info);
    void NewThread(const Napi::CallbackInfo& info);
    void NewUserData(const Napi::CallbackInfo& info);
    void Next(const Napi::CallbackInfo& info);
    void PCall(const Napi::CallbackInfo& info);
    void Pop(const Napi::CallbackInfo& info);
    void PushBoolean(const Napi::CallbackInfo& info);
    void PushJSClosure(const Napi::CallbackInfo& info);
    void PushJSFunction(const Napi::CallbackInfo& info);
    void PushFString(const Napi::CallbackInfo& info);
    void PushInteger(const Napi::CallbackInfo& info);
    void PushString(const Napi::CallbackInfo& info);
    void PushThread(const Napi::CallbackInfo& info);
    void PushValue(const Napi::CallbackInfo& info);
    void PushVFString(const Napi::CallbackInfo& info);
    void RawEqual(const Napi::CallbackInfo& info);
    void RawGet(const Napi::CallbackInfo& info);
    void RawGeti(const Napi::CallbackInfo& info);
    void RawSet(const Napi::CallbackInfo& info);
    void RawSeti(const Napi::CallbackInfo& info);
    void Reader(const Napi::CallbackInfo& info);
    void Register(const Napi::CallbackInfo& info);
    void Remove(const Napi::CallbackInfo& info);
    void Replace(const Napi::CallbackInfo& info);
    void Resume(const Napi::CallbackInfo& info);
    void SetAllocf(const Napi::CallbackInfo& info);
    void SetFenv(const Napi::CallbackInfo& info);
    void SetField(const Napi::CallbackInfo& info);
    void SetGlobal(const Napi::CallbackInfo& info);
    void SetMetatable(const Napi::CallbackInfo& info);
    void SetTable(const Napi::CallbackInfo& info);
    void SetTop(const Napi::CallbackInfo& info);
    void Status(const Napi::CallbackInfo& info);
    void ToBoolean(const Napi::CallbackInfo& info);
    void ToJSFunction(const Napi::CallbackInfo& info);
    Napi::Value ToInteger(const Napi::CallbackInfo& info);
    void ToLString(const Napi::CallbackInfo& info);
    void ToNumber(const Napi::CallbackInfo& info);
    void ToPointer(const Napi::CallbackInfo& info);
    void ToString(const Napi::CallbackInfo& info);
    void ToThread(const Napi::CallbackInfo& info);
    void ToUserData(const Napi::CallbackInfo& info);
    void Type(const Napi::CallbackInfo& info);
    void TypeName(const Napi::CallbackInfo& info);
    void XMove(const Napi::CallbackInfo& info);
    void Yield(const Napi::CallbackInfo& info);
    void GetHook(const Napi::CallbackInfo& info);
    void GetHookCount(const Napi::CallbackInfo& info);
    void GetHookMask(const Napi::CallbackInfo& info);
    void GetInfo(const Napi::CallbackInfo& info);
    void GetLocal(const Napi::CallbackInfo& info);
    void GetStack(const Napi::CallbackInfo& info);
    void GetUpValue(const Napi::CallbackInfo& info);
    void SetHook(const Napi::CallbackInfo& info);
    void SetLocal(const Napi::CallbackInfo& info);
    void SetUpValue(const Napi::CallbackInfo& info);

    Napi::Value DoString(const Napi::CallbackInfo& info);
    Napi::Value DoFile(const Napi::CallbackInfo& info);
    void OpenLibs(const Napi::CallbackInfo& info);
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

void LuaState::GetField(const Napi::CallbackInfo& info) {
    lua_getfield(L, GetArg<int>(info, 0), GetArg<const char*>(info, 1));
}

void LuaState::GetGlobal(const Napi::CallbackInfo& info) {
    lua_getglobal(L, GetArg<const char*>(info, 0));
}

void LuaState::NewTable(const Napi::CallbackInfo& info) {
    lua_newtable(L);
}

void LuaState::PushString(const Napi::CallbackInfo& info) {
    lua_pushstring(L, GetArg<const char*>(info, 0));
}

Napi::Value LuaState::ToInteger(const Napi::CallbackInfo& info) {
    int value = lua_tointeger(L, GetArg<int>(info, 0));
    return Napi::Number::New(info.Env(), value);
}

Napi::Value LuaState::IsNumber(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), lua_isnumber(L, GetArg<int>(info, 0)));
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
