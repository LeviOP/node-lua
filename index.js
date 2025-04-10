import { createRequire } from "module";
const require = createRequire(import.meta.url);
export const { LuaState, LUA } = require("./build/Release/node-lua.node");
