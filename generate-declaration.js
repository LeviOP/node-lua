import { LUA } from "./index.js";
import { copyFile } from "node:fs/promises";

async function getVersionFile() {
    switch (LUA.VERSION_NUM) {
        case 501:
            return "src/versions/51.d.ts";
    }
}

const delcarationFile = await getVersionFile();
await copyFile(delcarationFile, "index.d.ts");
