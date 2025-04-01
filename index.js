import { createRequire } from "module";
const require = createRequire(import.meta.url);
export const { LuaState } = require("./build/Release/addon.node");
