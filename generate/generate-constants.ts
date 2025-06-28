import { spawn } from "node:child_process";
import { join } from "node:path";
import { readFile, writeFile } from "node:fs/promises";

const pkgConfig = spawn("pkg-config", ["--cflags", "luajit"]);

let output = "";
pkgConfig.stdout.on("data", (data: Buffer) => {
    output += data.toString();
});

pkgConfig.stdout.on("close", async () => {
    const [path] = output.match(/(?<=-I)[^\n]*(?=\n)/g) ?? [null];
    if (path === null) throw Error("Didn't find path in pkg-config result!");

    const files = ["lua.h", "lauxlib.h"];

    const constants = (await Promise.all(files.map<Promise<string[]>>(async (filename) => {
        // I don't think utf-8 is used at all in these files, but as long as it's ascii it's fine
        const file = await readFile(join(path, filename), { encoding: "utf-8" });
        const results = file.match(/(?<=^#define\sLUA_)[^\s]*(?=\s+[^\n]*$)/gm);
        if (results === null) throw Error("Didn't find any define statements in " + filename + "!");
        return results;
    }))).flat();

    await writeFile("constants.json", JSON.stringify(constants, null, 4), { encoding: "utf-8" });
});
