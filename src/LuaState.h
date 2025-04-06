#ifndef luastate_h
#define luastate_h

#include <cassert>
#include <lua.hpp>
#include <napi.h>

#define NODELUA_DEFAULT_MSGHANDLER "nodelua_default_msghandler"
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
inline const char *GetArg<const char *>(const Napi::CallbackInfo &info,
                                        size_t index) {
    static thread_local std::string temp;
    temp = info[index].As<Napi::String>().Utf8Value();
    return temp.c_str();
}

class LuaState : public Napi::ObjectWrap<LuaState> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    LuaState(const Napi::CallbackInfo &info);
    ~LuaState();
    std::vector<Napi::FunctionReference> functions;
    std::vector<size_findex> freelist;
    lua_State *L;

private:
    void push_js_closure(Napi::Function function, int n);
    size_findex register_function(Napi::Function function);
    void callback();

    Napi::Value CallEx(int nargs, int nresults, bool catchError);

    Napi::Value Call(const Napi::CallbackInfo &info);
    Napi::Value CheckStack(const Napi::CallbackInfo &info);
    void Close(const Napi::CallbackInfo &info);
    void Concat(const Napi::CallbackInfo &info);
    // void CPCall(const Napi::CallbackInfo& info);
    void CreateTable(const Napi::CallbackInfo &info);
    // void Dump(const Napi::CallbackInfo& info);
    Napi::Value Equal(const Napi::CallbackInfo &info);
    void Error(const Napi::CallbackInfo &info);
    Napi::Value GC(const Napi::CallbackInfo &info);
    // void GetAllocF(const Napi::CallbackInfo& info);
    void GetFEnv(const Napi::CallbackInfo &info);
    void GetField(const Napi::CallbackInfo &info);
    void GetGlobal(const Napi::CallbackInfo &info);
    Napi::Value GetMetatable(const Napi::CallbackInfo &info);
    void GetTable(const Napi::CallbackInfo &info);
    Napi::Value GetTop(const Napi::CallbackInfo &info);
    void Insert(const Napi::CallbackInfo &info);
    Napi::Value IsBoolean(const Napi::CallbackInfo &info);
    Napi::Value IsJSFunction(const Napi::CallbackInfo &info);
    Napi::Value IsFunction(const Napi::CallbackInfo &info);
    Napi::Value IsLightUserdata(const Napi::CallbackInfo &info);
    Napi::Value IsNil(const Napi::CallbackInfo &info);
    Napi::Value IsNone(const Napi::CallbackInfo &info);
    Napi::Value IsNoneOrNil(const Napi::CallbackInfo &info);
    Napi::Value IsNumber(const Napi::CallbackInfo &info);
    Napi::Value IsString(const Napi::CallbackInfo &info);
    Napi::Value IsTable(const Napi::CallbackInfo &info);
    Napi::Value IsThread(const Napi::CallbackInfo &info);
    Napi::Value IsUserdata(const Napi::CallbackInfo &info);
    Napi::Value LessThan(const Napi::CallbackInfo &info);
    // Napi::Value Load(const Napi::CallbackInfo &info);
    void NewTable(const Napi::CallbackInfo &info);
    // void NewThread(const Napi::CallbackInfo &info);
    // void NewUserdata(const Napi::CallbackInfo &info);
    void Next(const Napi::CallbackInfo &info);
    Napi::Value ObjLen(const Napi::CallbackInfo &info);
    // void PCall(const Napi::CallbackInfo &info);
    void Pop(const Napi::CallbackInfo &info);
    void PushBoolean(const Napi::CallbackInfo &info);
    void PushJSClosure(const Napi::CallbackInfo &info);
    void PushJSFunction(const Napi::CallbackInfo &info);
    void PushFString(const Napi::CallbackInfo &info);
    void PushInteger(const Napi::CallbackInfo &info);
    void PushString(const Napi::CallbackInfo &info);
    void PushThread(const Napi::CallbackInfo &info);
    void PushValue(const Napi::CallbackInfo &info);
    void PushVFString(const Napi::CallbackInfo &info);
    void RawEqual(const Napi::CallbackInfo &info);
    void RawGet(const Napi::CallbackInfo &info);
    void RawGeti(const Napi::CallbackInfo &info);
    void RawSet(const Napi::CallbackInfo &info);
    void RawSeti(const Napi::CallbackInfo &info);
    void Reader(const Napi::CallbackInfo &info);
    void Register(const Napi::CallbackInfo &info);
    void Remove(const Napi::CallbackInfo &info);
    void Replace(const Napi::CallbackInfo &info);
    void Resume(const Napi::CallbackInfo &info);
    void SetAllocf(const Napi::CallbackInfo &info);
    void SetFenv(const Napi::CallbackInfo &info);
    void SetField(const Napi::CallbackInfo &info);
    void SetGlobal(const Napi::CallbackInfo &info);
    void SetMetatable(const Napi::CallbackInfo &info);
    void SetTable(const Napi::CallbackInfo &info);
    void SetTop(const Napi::CallbackInfo &info);
    void Status(const Napi::CallbackInfo &info);
    void ToBoolean(const Napi::CallbackInfo &info);
    void ToJSFunction(const Napi::CallbackInfo &info);
    Napi::Value ToInteger(const Napi::CallbackInfo &info);
    void ToLString(const Napi::CallbackInfo &info);
    void ToNumber(const Napi::CallbackInfo &info);
    void ToPointer(const Napi::CallbackInfo &info);
    void ToString(const Napi::CallbackInfo &info);
    void ToThread(const Napi::CallbackInfo &info);
    void ToUserData(const Napi::CallbackInfo &info);
    void Type(const Napi::CallbackInfo &info);
    void TypeName(const Napi::CallbackInfo &info);
    void XMove(const Napi::CallbackInfo &info);
    void Yield(const Napi::CallbackInfo &info);
    void GetHook(const Napi::CallbackInfo &info);
    void GetHookCount(const Napi::CallbackInfo &info);
    void GetHookMask(const Napi::CallbackInfo &info);
    void GetInfo(const Napi::CallbackInfo &info);
    void GetLocal(const Napi::CallbackInfo &info);
    void GetStack(const Napi::CallbackInfo &info);
    void GetUpValue(const Napi::CallbackInfo &info);
    void SetHook(const Napi::CallbackInfo &info);
    void SetLocal(const Napi::CallbackInfo &info);
    void SetUpValue(const Napi::CallbackInfo &info);

    Napi::Value DoString(const Napi::CallbackInfo &info);
    Napi::Value DoFile(const Napi::CallbackInfo &info);
    void OpenLibs(const Napi::CallbackInfo &info);
};

#endif
