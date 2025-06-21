import { JSDOM } from "jsdom";
import { readFile, writeFile } from "node:fs/promises";
import Parser from "tree-sitter";
import C from "tree-sitter-c";
import { NAMEMAP, IGNORELIST } from "./constants.js"

export interface Declaration {
    name: string;
    type: string;
    isPointer: boolean;
}

export interface CFunction {
    declaration: Declaration;
    parameters: Declaration[];
}

export interface Method {
    fragment: string;
    indicator: string;
    cFunction: CFunction;
    jsName: string;
    doc: string;
}

const parser = new Parser();
// ??????????????????????????????????????
parser.setLanguage(C as Parser.Language);

function evaluateDeclarator(source: Parser.SyntaxNode): [Parser.SyntaxNode, boolean, undefined];
function evaluateDeclarator(source: Parser.SyntaxNode, valueName: string): [Parser.SyntaxNode, boolean, Parser.SyntaxNode];
function evaluateDeclarator(source: Parser.SyntaxNode, valueName?: string): [Parser.SyntaxNode, boolean, Parser.SyntaxNode | undefined] {
    let declaratorValue = source;
    let isPointer = false;

    if (source.type === "pointer_declarator") {
        const maybe = source.childForFieldName("declarator");
        if (maybe === null) throw Error("pointer_declarator didn't have declarator??");
        declaratorValue = maybe;
        isPointer = true;
    }

    let value: Parser.SyntaxNode | undefined = undefined;
    if (valueName !== undefined) {
        const maybe = declaratorValue.childForFieldName(valueName);
        if (maybe === null) throw Error("couldn't find named value field while parsing declarator");
        value = maybe;
    }

    return [declaratorValue, isPointer, value];
}

function parseDeclaration(source: string): CFunction | null {
    const tree = parser.parse(source);
    const thing = tree.rootNode.firstChild;
    if (thing === null) throw Error("what? why doesn't he have a child?");
    if (thing.type !== "declaration") return null;

    const typeNode = thing.childForFieldName("type");
    if (typeNode === null) throw Error("what? why doesn't he (second) have a child?");

    const type = typeNode.text;

    const declarationDeclarator = thing.childForFieldName("declarator");
    if (declarationDeclarator === null) throw Error("What? why doesn't this CRAP exist???");
    const [functionDeclarator, isPointer, parameterList] = evaluateDeclarator(declarationDeclarator, "parameters");

    const nameNode = functionDeclarator.childForFieldName("declarator");
    if (nameNode === null) throw Error("named declarator node didn't exist for some reason");
    const name = nameNode.text;

    if (parameterList === null) throw Error("No named child 'parameters'");
    const parameters = parameterList.namedChildren.map<Declaration>((parameter) => {
        if (parameter.type !== "parameter_declaration") throw Error("Unexpected node type in parameter list");
        const typeNode = parameter.childForFieldName("type");
        if (typeNode === null) throw Error("Expected type field");
        const type = typeNode.text;

        const declarator = parameter.childForFieldName("declarator");
        if (declarator === null) throw Error("Expected declarator field" + source);
        const [nameNode, isPointer] = evaluateDeclarator(declarator);
        const name = nameNode.text;
        return {
            type,
            name,
            isPointer
        };
    });

    return {
        declaration: {
            name,
            type,
            isPointer
        },
        parameters
    }
}

function parseParagraph(nodeList: NodeListOf<ChildNode> | [Node]): string {
    return Array.from(nodeList).map((node) => {
        if (node instanceof window.Text) {
            return node.data;
        } else if (node instanceof window.HTMLParagraphElement || node instanceof window.HTMLUListElement) {
            return parseParagraph(node.childNodes);
        } else if (node instanceof window.HTMLLIElement) {
            return `* ${parseParagraph(node.childNodes)}\n`;
        } else if (node instanceof window.HTMLAnchorElement) {
            if (node.href === "") return parseParagraph(node.childNodes);
            return `[${parseParagraph(node.childNodes)}](${node.href})`;
        } else if (node instanceof window.HTMLElement) {
            if (node.tagName === "EM") {
                return `*${parseParagraph(node.childNodes)}*`;
            } else if (node.tagName === "CODE") {
                return `\`${parseParagraph(node.childNodes)}\``;
            } else if (node.tagName === "B") {
                return `**${parseParagraph(node.childNodes)}**`;
            } else {
                console.log("Unhandled node type:", node.tagName);
                return node.textContent ?? "";
            }
        } else {
            console.log("unhandled node type:", node);
            return node.textContent ?? "";
        }
    }).join("");
}

const response = await fetch("https://www.lua.org/manual/5.1/manual.html");
const html = await response.text();
// const html = await readFile("manual.html", { encoding: "latin1" });
const { window } = new JSDOM(html, { url: "https://www.lua.org/manual/5.1/manual.html" });

const elements = window.document.body.children;

let i = 0;
for (let curr: Element | null; !(
    (curr = elements[i], curr instanceof window.HTMLHeadingElement) &&
    (curr = curr.firstElementChild, curr instanceof window.HTMLAnchorElement) &&
    (curr.innerHTML === "Functions and Types")
); i++);
do i++; while (!(elements[i] instanceof window.HTMLHeadingElement));
// for (; !(elements[i] instanceof window.HTMLHRElement); ++i);

const methods: Array<Method> = [];

while (true) {
    if (elements[i] instanceof window.HTMLHRElement) i++;

    const header = elements[i];
    if (!(header instanceof window.HTMLHeadingElement)) {
        console.log("Expected header!");
        break;
    }
    if (header.tagName !== "H3") {
        if (header.textContent.endsWith("Standard Libraries")) break;
        do i++; while (!(elements[i] instanceof window.HTMLHeadingElement));
        continue;
    }

    const anchor = header.firstElementChild;
    if (!(anchor instanceof window.HTMLAnchorElement)) {
        console.log("Child of header wasn't an anchor?");
        break;
    };
    const fragment = anchor.name;
    if (IGNORELIST.includes(fragment)) {
        for (; !(elements[i] instanceof window.HTMLHRElement || elements[i].tagName === "H2"); i++);
        continue;
    }

    const indicatorWrapper = elements[++i];
    if (!(indicatorWrapper instanceof window.HTMLParagraphElement)) {
        console.log("skipping something without indicator:", fragment);
        for (; !(elements[i] instanceof window.HTMLHRElement || elements[i] instanceof window.HTMLHeadingElement); i++);
        continue;
    }

    const indicatorElement = indicatorWrapper.firstElementChild;
    if (!(indicatorElement instanceof window.HTMLSpanElement)) {
        console.log("paragraph after header didn't have span child (supposedly for indicator)");
        break;
    }
    const indicator = indicatorElement.textContent!;

    const pre = elements[++i];
    if (!(pre instanceof window.HTMLPreElement)) {
        console.log("Expected pre!");
        break;
    }

    const cFunction = parseDeclaration(pre.innerHTML);
    if (cFunction === null) throw Error("what is going on here");

    let jsName = NAMEMAP.get(cFunction.declaration.name);
    if (jsName === undefined) {
        console.log("Name for thing didn't exist!!!!!!!!!", cFunction.declaration.name);
        jsName = cFunction.declaration.name.slice(4);
    }

    const paragraphs: Array<string> = [];
    for (let element = elements[++i]; !(element instanceof window.HTMLHRElement || element instanceof window.HTMLHeadingElement); i++, element = elements[i]) {
        if (element instanceof window.HTMLPreElement) {
            if (!element.innerHTML.endsWith("\n")) console.log("Expected code block to end with an extra newline?", i);
            const text = element.innerHTML.slice(0, -1).replace(/\/\* ?(.*?) ?\*\//g, "// $1");
            paragraphs.push(`\`\`\`c\n${text}\n\`\`\``);
        } else if (element instanceof window.HTMLUListElement) {
            paragraphs.push(Array.from(element.children).map((child) => parseParagraph([child]).trim().replace(/\s+/g, " ")).join("\n"));
        } else {
            paragraphs.push(parseParagraph([element]).trim().replace(/\s+/g, " "));
        }
    }
    const doc = paragraphs.reduce((output, paragraph, index) => {
        if (index === 0) return paragraph;
        if (!paragraph.startsWith("```") && !output.endsWith("```")) paragraph = "\n" + paragraph;
        return output + "\n" + paragraph;
    }, "");

    methods.push({
        fragment,
        indicator,
        cFunction,
        jsName,
        doc
    });
}

await writeFile("methods.json", JSON.stringify(methods, null, 4));
