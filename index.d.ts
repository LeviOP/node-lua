export class LuaState {
    openLibs(): void;
    getField(index: number, k: string): void;
    getGlobal(name: string): void;
    pushString(str: string): void;
    doString(str: string): string | null;
    doFile(filename: string): string | null;
    newTable(): void;
}
