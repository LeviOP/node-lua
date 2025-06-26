import ts from "typescript";
import { readFile, writeFile } from "node:fs/promises";
import { Method } from "./generate-definitions.js";
import { HARDCODED } from "./constants.js";

const methodsRaw = await readFile("./methods.json", { encoding: "utf-8" });
const methods: Method[] = JSON.parse(methodsRaw) as Method[];

interface Parameter {
    name: string;
    type: ts.TypeNode;
}

interface JSDoc {
    fragment: string;
    indicator: string;
    doc: string;
}

export interface MethodDeclaration {
    name: string;
    parameters: Parameter[];
    returnType: ts.TypeNode;
    jsdoc: JSDoc;
}

const methodDeclarations: MethodDeclaration[] = [...methods.map<MethodDeclaration>((method) => ({
    name: method.jsName,
    parameters: method.cFunction.parameters.slice(1).map((parameter) => ({
        name: parameter.name,
        type: cTypeToTypeNode(parameter.type)
    })),
    returnType: cTypeToTypeNode(method.cFunction.declaration.type),
    jsdoc: {
        fragment: method.fragment,
        indicator: method.indicator,
        doc: method.doc
    }
})), ...HARDCODED]

function stringToJSDoc(str: string) {
    return ("*\n" + str).split("\n").join("\n * ") + "\n ";
}

function cTypeToTypeNode(type: string): ts.TypeNode {
    switch (type) {
        case "lua_CFunction":
            return ts.factory.createTypeReferenceNode("JSFunction");
        case "size_t":
        case "lua_Integer":
        case "lua_Number":
        case "int":
            return ts.factory.createKeywordTypeNode(ts.SyntaxKind.NumberKeyword);
        case "void":
            return ts.factory.createKeywordTypeNode(ts.SyntaxKind.VoidKeyword);
        case "char":
            return ts.factory.createKeywordTypeNode(ts.SyntaxKind.StringKeyword);
        default:
            console.log("unknown c type:", type);
            return ts.factory.createTypeReferenceNode(type);
    }
}

const typeAliasDeclaration = ts.addSyntheticLeadingComment(ts.factory.createTypeAliasDeclaration(
    [ts.factory.createModifier(ts.SyntaxKind.ExportKeyword)],
    "JSFunction",
    undefined,
    ts.factory.createFunctionTypeNode(
        undefined,
        [
            ts.factory.createParameterDeclaration(
                undefined,
                undefined,
                "L",
                undefined,
                ts.factory.createTypeReferenceNode("LuaState")
            )
        ],
        ts.factory.createKeywordTypeNode(ts.SyntaxKind.NumberKeyword)
    )
), ts.SyntaxKind.MultiLineCommentTrivia, stringToJSDoc("lua_CFunction\n\nType for JS functions.\n\nIn order to communicate properly with Lua, a JS function must use the following protocol, which defines the way parameters and results are passed: a JS function receives its arguments from Lua in its stack in direct order (the first argument is pushed first). So, when the function starts, `lua_gettop(L)` returns the number of arguments received by the function. The first argument (if any) is at index 1 and its last argument is at index `lua_gettop(L)` To return values to Lua, a JS function just pushes them onto the stack, in direct order (the first result is pushed first), and returns the number of results. Any other value in the stack below the results will be properly discarded by Lua. Like a Lua function, a JS function called by Lua can also return many results.\n\nAs an example, the following function receives a variable number of numerical arguments and returns their average and sum:\n```c\n     static int foo (lua_State *L) {\n       int n = lua_gettop(L);    // number of arguments\n       lua_Number sum = 0;\n       int i;\n       for (i = 1; i &lt;= n; i++) {\n         if (!lua_isnumber(L, i)) {\n           lua_pushstring(L, \"incorrect argument\");\n           lua_error(L);\n         }\n         sum += lua_tonumber(L, i);\n       }\n       lua_pushnumber(L, sum/n);        // first result\n       lua_pushnumber(L, sum);         // second result\n       return 2;                   // number of results\n     }\n```\n@see {@link https://www.lua.org/manual/5.1/manual.html#lua_CFunction}"), true);

const classDeclaration = ts.factory.createClassDeclaration(
    [ts.factory.createModifier(ts.SyntaxKind.ExportKeyword)],
    "LuaState",
    [],
    [],
    methodDeclarations.map((method) => ts.addSyntheticLeadingComment(ts.factory.createMethodDeclaration(
        undefined,
        undefined,
        method.name,
        undefined,
        undefined,
        method.parameters.map((parameter) => ts.factory.createParameterDeclaration(undefined, undefined, parameter.name, undefined, parameter.type)),
        method.returnType,
        undefined
    ), ts.SyntaxKind.MultiLineCommentTrivia, stringToJSDoc(`${method.jsdoc.fragment} -> ${method.jsdoc.indicator.replace(/([\[\]])/g, "\\$&")}\n\n${method.jsdoc.doc}\n@see {@link https://www.lua.org/manual/5.1/manual.html#${method.jsdoc.fragment}}`), true))
);

const sourceFile = ts.factory.createSourceFile(
    [typeAliasDeclaration, classDeclaration],
    ts.factory.createToken(ts.SyntaxKind.EndOfFileToken),
    ts.NodeFlags.None
);

const printer = ts.createPrinter({ newLine: ts.NewLineKind.LineFeed });
const result = printer.printFile(sourceFile);
// result += printer.printNode(ts.EmitHint.Unspecified, typeAliasDeclaration, sourceFile);
// result += printer.printNode(ts.EmitHint.Unspecified, classDeclaration, sourceFile);
await writeFile("index.d.ts", result);
