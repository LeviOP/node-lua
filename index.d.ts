export class LuaState {
    openLibs(): void;
    createTable(narr: number, nrec: number): void;
    setField(index: number, k: string): void
    getField(index: number, k: string): void;
    getGlobal(name: string): void;
    pushString(str: string): void;
    doString(str: string): string | null;
    doFile(filename: string): string | null;
    newTable(): void;
    call(nargs: number, nresults: number): null;
    pushJSFunction(func: (state: LuaState) => number): void;
    register(name: string, func: (state: LuaState) => number): void;
}
