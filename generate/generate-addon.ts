import { readFile, writeFile } from "node:fs/promises";
import { Method } from "./generate-definitions.js";

function cTypeToNapiName(type: string): string {
    switch (type) {
        case "size_t":
        case "lua_Integer":
        case "lua_Number":
        case "int":
            return "Number";
        case "char":
            return "String";
        default:
            throw Error("unknown c type: " + type);
    }
}

const methodsRaw = await readFile("./methods.json", { encoding: "utf-8" });
const methods = JSON.parse(methodsRaw) as Method[];

const methodDeclarations = methods.map((method) => (method.cFunction.declaration.type === "void" ? "void" : "Napi::Value") +
` ${method.jsName.charAt(0).toUpperCase() + method.jsName.slice(1)}(const Napi::CallbackInfo& info) {
    ${method.cFunction.declaration.type === "void" ? "" : "return Napi::" + cTypeToNapiName(method.cFunction.declaration.type) +
`::New(
        info.Env(),
        `}${method.fragment}(${["L", ...method.cFunction.parameters.slice(1).map((param, i) =>
`GetArg<${
    param.type === "char" ? param.type = "const char" : undefined,
    param.type + (param.isPointer ? "*" : "")
}>(info, ${i})`)].join(", ")})${method.cFunction.declaration.type === "void" ? "" : "\n    )"};
}`).join("\n");

const properties = methods.map((method) => `InstanceMethod("${method.jsName}", &LuaState::${method.jsName.charAt(0).toUpperCase() + method.jsName.slice(1)})`).join(",\n");

const constantsRaw = await readFile("./constants.json", { encoding: "utf-8" });
const constants = JSON.parse(constantsRaw) as string[];

const constantsList = constants.map((constant) => `ADD_CONSTANT(${constant});`).join("\n");

writeFile("src/methods.h", methodDeclarations);
writeFile("src/properties.h", properties);
writeFile("src/constants.h", constantsList);
