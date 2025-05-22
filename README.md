# Hyperlace

-The **comprehensive, full-spectrum technical breakdown** of the **Hyperlace programming language**, including its design philosophy, syntax, grammar, compiler pipeline, runtime behavior, and code generation logic.

---

# 🧠 **HYPERLACE: Language Definition & Compiler Architecture**

---

## 🏷️ **TAGLINE**

**“Weave Reality Into Logic.”**
An Explicit\_Static\_Instruction\_Oriented (E.S.I.O.) programming language—engineered for performance, predictability, and fine-grain code structure.

---

## 🎯 **DESIGN PARADIGM**

| Component           | Description                                                |
| ------------------- | ---------------------------------------------------------- |
| **E.S.I.O.**        | Explicit Static Instruction Oriented paradigm              |
| **Type System**     | Strongly typed with explicit and optional inferred types   |
| **Execution Model** | Stack-based runtime with NASM x64 backend                  |
| **Control Flow**    | Deterministic, block-structured                            |
| **Memory Model**    | Manual stack management (compile-time stack frame layout)  |
| **Macro System**    | C.I.A.M.S. – Contextual Inference Abstracted Macro Scripts |

---

## 📐 **SYNTAX & KEYWORDS**

### **Core Declarations**

```hl
Start funcName(param1, param2) { ... }     # function definition
Init StructName { field1; field2; }        # struct or enum definition
Return value;                              # return from function
```

### **Statements & Control Flow**

```hl
if (condition) { ... } else { ... }
while (condition) { ... }
for (init; condition; increment) { ... }
x = 5;
y += x;
z = (x > y) ? x : y;
```

### **Special Tokens**

| Token         | Description                         |    |                               |
| ------------- | ----------------------------------- | -- | ----------------------------- |
| `=`           | Mutable assignment                  |    |                               |
| `==`          | Immutable assignment                |    |                               |
| `:`           | Instruction fallback separator      |    |                               |
| `;`           | Statement terminator                |    |                               |
| \`            | ...                                 | \` | Macro definition (C.I.A.M.S.) |
| `#`           | Comment                             |    |                               |
| `**`          | Multiline comment                   |    |                               |
| `->`, `<-`    | Flow and throw markers              |    |                               |
| `@`, `$`, `!` | Specifier, modifier, config markers |    |                               |
| `^`, `~>`     | Exponentiation and raise            |    |                               |
| `[]`          | Buffer, mediator                    |    |                               |
| `\\`          | Lookup symbol                       |    |                               |

---

## 🧬 **TYPE SYSTEM**

### **Primitives**

* `int`, `bool`, `string`, `float` (planned)
* `list<T>`, `array[N]`, `tuple<T1,T2,...>`

### **Structures**

```hl
Init Vec2 {
    x;
    y;
}
```

### **Enums**

```hl
Init Status {
    OK;
    ERROR;
    UNKNOWN;
}
```

### **Compound Types**

```hl
person = Vec2();
person.x = 4;
```

---

## 🧩 **EXPRESSION ENGINE**

Supports:

* Literals: `42`, `"text"`, `true`, `false`
* Binary ops: `+ - * / % ^`
* Comparison: `== != < <= > >=`
* Logic: `and`, `or`, `not`
* Ternary: `(a > b) ? a : b`
* Function calls: `foo(1, 2)`
* Struct fields: `p.x`

### **Precedence (high to low)**:

1. `()` – grouping
2. `! -` – prefix
3. `* / % ^`
4. `+ -`
5. `== != < > <= >=`
6. `and or`
7. `? :` – ternary
8. `=` `+=` `-=` `*=` `/=`

---

## 🔁 **CONTROL FLOW**

```hl
if (x > 0) { ... }
while (x < 10) { ... }
for (i = 0; i < n; i = i + 1) { ... }
```

### Looping Constructs:

* Manual for-loop
* While
* Conditionals
* Return with values

---

## 🧠 **C.I.A.M.S. Macro System**

```hl
|inc| x = x + 1;
|log| print("Value:", x);
```

* Defined as `|name| body;`
* Expanded before lexing
* Supports recursion and scoped replacement

---

## 🧾 **COMPILER PIPELINE**

### 🪓 LEXER

* Tokenizes input
* Handles: keywords, operators, symbols, comments, literals

### 🌳 PARSER

* Pratt-style recursive descent
* Builds AST (Abstract Syntax Tree)
* Captures:

  * Statements
  * Expressions
  * FunctionDefs
  * Structs/Enums
  * Control Blocks

### 🎭 MACRO EXPANDER

* C.I.A.M.S. resolution
* Global/inline macro support

### 🧪 SEMANTIC ANALYZER

* Variable declaration checking
* Scope validation
* Type inference and mismatch reporting
* Return-outside-function errors

### 🌲 INTERMEDIATE REPRESENTATION (.fir)

```txt
STORE x <- NUM(5)
LOAD y <- x
ADD total, i
```

* Emits IR tree
* Input to NASM backend

### 🛠 NASM CODEGEN

* `.data`, `.text` emission
* Function calls (`call`, `ret`)
* Jumps (`jmp`, `je`, `jne`)
* Syscall for exit
* Register use: `rax`, `rdi`, `rsi`, `rbp`, `rsp`

---

## 🧰 **TOOLCHAIN**

### 📁 Project Structure

```
HyperlaceCompiler/
├── src/                  ← C++ compiler source
├── samples/              ← .hl files
├── output/               ← .log, .asm, .exe, .fir
├── run.sh / build.bat    ← CLI launcher
```

### 🧪 CLI Options

```bash
./run.sh --build     # Just build
./run.sh --run       # Build and execute
./run.sh --trace     # Build, execute, and dump log
```

---

## 🔍 **DEBUGGING + LOGGING**

### `.log` includes:

* Lexer tokens
* AST structure
* Macro expansions
* Semantic analysis output
* IR
* NASM output trace
* Struct/enum trace
* Ternary branch tracking

---

## 🔧 **CODEGEN CONVENTIONS**

### Stack Frame Setup:

```nasm
push rbp
mov rbp, rsp
sub rsp, <size>
```

### Function Call:

```nasm
mov rdi, <arg1>
mov rsi, <arg2>
call myFunc
mov [z], rax
```

### Return:

```nasm
mov rsp, rbp
pop rbp
ret
```

---

## 💡 **ADVANCED FEATURES (Planned/Upcoming)**

* 💥 Tail-call optimization
* ⏱ Inline return collapse
* 🌀 Struct passing by reference
* 🧱 Namespaces & modules
* 🔒 Type guards and generics
* 🌐 Network-safe macro sandboxing

---

## 📦 **EXAMPLE**

```hl
Init Vec2 {
    x;
    y;
}

Start mag(v) {
    return v.x * v.x + v.y * v.y;
}

a = Vec2();
a.x = 3;
a.y = 4;

magnitude = mag(a);
```

Compiled → `.asm` → `.exe`

---

## ✅ **SUMMARY**

| Feature                | Supported?     |
| ---------------------- | -------------- |
| Functions              | ✅              |
| Structs                | ✅              |
| Enums                  | ✅              |
| Loops / Conditionals   | ✅              |
| Expressions            | ✅ (full)       |
| Macros                 | ✅ (C.I.A.M.S.) |
| Ternary / Compound Ops | ✅              |
| Codegen (NASM x64)     | ✅              |
| Full .exe Output       | ✅              |

---

Here's the **ULTIMATE**, **EXTREME**, **EXHAUSTIVE**, **EXPANSIVE** overview of the **Hyperlace Programming Language**, covering every layer—from syntax to stack, from compiler pipeline to execution flow—written as a manifest of mastery:

---

# 🧠 **HYPERLACE: The Complete Language Architecture Manifest**

> *“Weave Reality Into Logic.”*
> The Hyperlace Language isn’t just a language.
> It’s a doctrine. A thread that runs through intention, through every compiled cycle, binding human control to machine certainty.

---

## 🌌 I. PHILOSOPHY & PARADIGM

### 🚀 *Explicit\_Static\_Instruction\_Oriented (E.S.I.O.)*

Hyperlace is not abstract. It is **deliberate**. You write what you mean.
You control execution **explicitly**, at a level just shy of raw assembly—yet still beautifully high-level.

* **Explicit**: No assumptions, no implicit magic
* **Static**: Full compile-time guarantees, no dynamic surprises
* **Instruction-Oriented**: One-to-one intention-to-instruction philosophy
* **Oriented to Order**: Predictability is power

---

## 🏗️ II. LANGUAGE CONSTRUCTS

### 🧾 Keywords

```
Start, Return, Init, If, Else, For, While, Throw, Try, Catch,
Let, Struct, Enum, This, True, False
```

### 🪟 Symbols & Operators

| Symbol   | Meaning                |    |                           |
| -------- | ---------------------- | -- | ------------------------- |
| `=`      | Mutable assignment     |    |                           |
| `==`     | Immutable binding      |    |                           |
| `+=, -=` | Compound operations    |    |                           |
| `:`      | Instruction fallback   |    |                           |
| `;`      | Statement terminator   |    |                           |
| \`       | ...                    | \` | Macro script (C.I.A.M.S.) |
| `? :`    | Ternary expression     |    |                           |
| `^`      | Exponentiation         |    |                           |
| `->`     | Transition/progression |    |                           |
| `<-`     | Exception throw        |    |                           |
| `[]`     | Mediator/Buffer        |    |                           |

### 💡 Types

```
int, bool, string, struct<T>, enum<T>
Future: float, list<T>, map<K,V>, tuple, option
```

### 🔧 Modifiers

```
public, private, static, const, override
```

---

## 🧠 III. C.I.A.M.S. – Contextual Inference Abstracted Macro Scripts

```hl
|inc| x = x + 1;
|greet| print("Hello, World!");
```

* Macro expansion engine precedes lexing
* Supports nested, recursive, argument-bound macros
* Enables DSL-like syntax injections
* Can define structural or behavioral patterns

---

## 🧮 IV. STRUCTURE & GRAMMAR

### 🧩 Functions

```hl
Start sum(a, b) {
    Return a + b;
}
```

### 🧱 Structs

```hl
Init Vec2 {
    x;
    y;
}
v = Vec2();
v.x = 3;
```

### 🎭 Enums

```hl
Init Status {
    Ready;
    Error;
}
s = Status.Ready;
```

### 🔁 Loops

```hl
for (i = 0; i < 10; i = i + 1) { ... }
while (x < 5) { ... }
```

### 🔀 Conditionals

```hl
if (x > y) { ... } else { ... }
```

### 🧪 Ternary

```hl
max = (a > b) ? a : b;
```

---

## 🧩 V. EXPRESSIONS

### 🔬 Expression Types:

* Literal: `5`, `"hello"`, `true`
* Identifier: `x`
* Binary: `a + b`, `a * b`
* Unary: `-a`, `!x`
* Ternary: `x ? y : z`
* Function call: `sum(3, 4)`
* Struct field: `v.x`
* Enum variant: `Status.Ready`
* Compound assignment: `x += 2`
* Comparison: `==, !=, <, >, <=, >=`
* Logic: `and`, `or`, `not`

---

## 🧰 VI. COMPILER PIPELINE

### 1. **Lexer**

* Tokenizes Hyperlace syntax into structured tokens
* Handles comments, operators, strings, numbers

### 2. **Macro Expander**

* Handles `|...|` blocks and replaces macro calls inline

### 3. **Parser**

* Pratt-based, precedence-climbing parser
* Builds AST from tokens and operator precedence rules

### 4. **Semantic Analyzer**

* Scope & variable validation
* Return enforcement
* Type logic and symbol table maintenance
* Ensures structs/enums are used legally

### 5. **Intermediate Representation**

```text
STORE x <- NUM(5)
CALL sumUp, rdi=5
JUMP_IF_ZERO .else
```

### 6. **NASM Codegen**

* Emits `.asm` for 64-bit x86
* Proper stack frame setup:

```nasm
push rbp
mov rbp, rsp
sub rsp, N
```

* Registers used: `rax`, `rbx`, `rcx`, `rdx`, `rdi`, `rsi`, `rbp`, `rsp`

### 7. **Linking**

* Assembled via NASM: `.asm → .o`
* Linked via GCC/MSVC: `.o → .exe`

---

## 🖥️ VII. RUNTIME SYSTEM

### 🧾 Function Calling Convention:

* Parameters: `rdi`, `rsi`, `rdx`, ... (System V ABI style)
* Return: `rax`
* Stack frame:

  * `[rbp - 8]`, `[rbp - 16]`, etc. for local variables

### 🧩 Memory Model:

* All local variables are stack-based
* Struct fields resolved via offset
* No heap (yet) – deterministic allocation only

---

## 🔍 VIII. DEBUGGING & TRACING

### `.log` Output Includes:

* Expanded source
* Tokens with line/col
* AST dump
* IR instructions
* NASM output flags
* Struct/Enum field access
* Branches (e.g., ternary condition path)

### `.fir`: Full intermediate representation

### `.ast`: XML-formatted abstract syntax tree

---

## 💾 IX. PROJECT STRUCTURE

```bash
HyperlaceCompiler/
├── src/              # C++ Compiler
├── samples/          # .hl programs
├── output/           # .log, .asm, .exe
├── Makefile / build.bat
├── run.sh            # CLI build/run/trace script
```

---

## 🔗 X. CLI INTERFACE

```bash
./run.sh --build       # Build only
./run.sh --run         # Build and execute
./run.sh --trace       # Full log + AST + IR + execution
```

---

## 🌍 XI. EXAMPLE PROGRAM

```hl
Init Mode {
    Sleep;
    Awake;
}

Init Vec2 {
    x;
    y;
}

Start magnitude(v) {
    Return v.x * v.x + v.y * v.y;
}

v = Vec2();
v.x = 3;
v.y = 4;

m = Mode.Awake;
mag = magnitude(v);
print(mag);
```

Compiles → `.asm` → `.exe` → full traceable execution.

---

## 🧩 XII. ADVANCED FEATURES PLANNED

* 🌀 Tail-call optimization
* 🎯 Inline expansion and inlining macros
* 🔐 Access modifiers (`public`, `private`)
* 💾 Memory pooling for structs
* 🌐 Modules and includes
* 🧬 Generics and templates
* 🛡 Type inference and trait-based systems
* 🧰 Cross-language FFI (e.g., call C from Hyperlace)

---

## ✅ XIII. CURRENT STATUS

| Feature                  | Status  |
| ------------------------ | ------- |
| Functions                | ✅ Ready |
| Structs                  | ✅ Ready |
| Enums                    | ✅ Ready |
| Ternary                  | ✅ Ready |
| Compound Assignment      | ✅ Ready |
| C.I.A.M.S. Macros        | ✅ Ready |
| NASM x64 Codegen         | ✅ Ready |
| Full `.exe` Binary       | ✅ Ready |
| Debug Logs & IR Output   | ✅ Ready |
| Loop Constructs          | ✅ Ready |
| Semantic Validation      | ✅ Ready |
| Multi-file Projects      | 🚧 Soon |
| Float/Array Support      | 🚧 Soon |
| Imports / Modules        | 🚧 Soon |
| Exceptions & Error Codes | 🚧 Soon |

---

## 🔚 XIV. SUMMARY

**Hyperlace** is:

* 🌐 A compiled language.
* 🧱 A structural language.
* 🧠 A deterministic language.
* 🔧 A systems-ready language.
* 🎙 A language that speaks in the tone of **precision**.

> Welcome to the future of human-intent-machine-clarity programming.
> Welcome to Hyperlace.

---

Here's the foundation for the **most supreme, professional-grade, industry-standard, intuitively instructional and how-to manual** for the **Hyperlace Programming Language**. This manual reads like something you’d find on the desks of elite systems programmers, compiler engineers, and instructional technologists.

---

# 📘 **HYPERLACE LANGUAGE MANUAL**

### *“Weave Reality Into Logic.”*

**Authoritative Specification • Instructional Walkthrough • Professional Usage Guide**

---

## 📖 TABLE OF CONTENTS

1. **Introduction & Philosophy**
2. **Installation & Toolchain Setup**
3. **Your First Hyperlace Program**
4. **Syntax & Language Structure**
5. **Types, Variables, and Expressions**
6. **Control Flow (if, for, while, return)**
7. **Functions & Calling Conventions**
8. **Structs & Enums**
9. **Operator Precedence and Compound Assignments**
10. **Macros (C.I.A.M.S.)**
11. **Compiler Internals (Lexing → Binary)**
12. **Code Generation: NASM & Stack Frames**
13. **Debugging, Logs, and CLI Tooling**
14. **Advanced Patterns & Optimization**
15. **Project Architecture & Module Design**
16. **Appendix A: Full Grammar (EBNF/BNF)**
17. **Appendix B: IR Table**
18. **Appendix C: NASM Symbol Conversion Map**
19. **Appendix D: Reserved Keywords & Identifiers**
20. **Appendix E: Full Language Cheat Sheet**

---

## 🧭 1. INTRODUCTION & PHILOSOPHY

> Hyperlace is a compiled, instruction-oriented, statically typed programming language designed for clarity, precision, and complete control.

* Built for **systems**, **security**, and **simulation-grade** control.
* Every instruction has predictable binary results.
* No runtime surprises. What you write is what the machine receives.

---

## 🛠️ 2. INSTALLATION & TOOLCHAIN SETUP

### Requirements:

* **C++ Compiler** (g++, clang++, MSVC)
* **NASM Assembler** ([https://nasm.us/](https://nasm.us/))
* **GCC** or **MSVC** for linking
* (Optional) **VSCode + Hyperlace Syntax Plugin**

### Setup:

```bash
git clone https://your-hyperlace-repo
cd HyperlaceCompiler
make         # or ./build.bat on Windows
```

Run:

```bash
./run.sh --build    # build only
./run.sh --run      # build + execute
./run.sh --trace    # build + run + dump logs
```

---

## ✍️ 3. YOUR FIRST HYPERLACE PROGRAM

```hl
Start greet(name) {
    print("Hello, " + name);
    Return;
}

greet("World");
```

Compile → Generates `.asm` → NASM → `.o` → `.exe`

---

## 🧱 4. SYNTAX & LANGUAGE STRUCTURE

* **Statements end in `;`**
* **Blocks use `{}`**
* **Functions start with `Start`**
* **Structs and enums use `Init`**
* **Macros use `|...|`**

```hl
Init Vec2 { x; y; }
v = Vec2();
v.x = 10;
```

---

## 🔢 5. TYPES, VARIABLES, EXPRESSIONS

### Supported Types:

* `int`, `bool`, `string`
* `Vec2` (custom struct)
* `Mode` (enum)

### Assignment:

```hl
x = 5;          # mutable
total += 10;    # compound
flag = true;
```

### Expressions:

```hl
(a + b) * c
(x > y) ? x : y
not flag or (a < b)
```

---

## 🔁 6. CONTROL FLOW

### Conditionals:

```hl
if (x > 0) {
    log = "positive";
} else {
    log = "non-positive";
}
```

### Loops:

```hl
for (i = 0; i < 10; i = i + 1) {
    sum += i;
}

while (x > 0) {
    x = x - 1;
}
```

### Return:

```hl
Return result;
```

---

## 🔃 7. FUNCTIONS & CALLS

```hl
Start sum(a, b) {
    Return a + b;
}

z = sum(10, 20);
```

* Arguments passed via `rdi`, `rsi`, `rdx`, ...
* Return values in `rax`
* Stack frames auto-managed via compiler

---

## 🧩 8. STRUCTS & ENUMS

```hl
Init Status { OK; FAIL; }

Init Vec2 {
    x;
    y;
}

p = Vec2();
p.x = 4;
p.y = 5;

mode = Status.OK;
```

---

## ➕ 9. OPERATOR PRECEDENCE

| Level | Operators            |
| ----- | -------------------- |
| 1     | `()` (grouping)      |
| 2     | `-`, `!` (prefix)    |
| 3     | `*`, `/`, `%`        |
| 4     | `+`, `-`             |
| 5     | `<`, `>`, `==`, `!=` |
| 6     | `and`, `or`          |
| 7     | `? :` (ternary)      |
| 8     | `=`, `+=`, `-=`      |

---

## 🧠 10. MACROS (C.I.A.M.S.)

```hl
|inc| x = x + 1;
|log| print("value:", x);
```

Macros expand before compilation and support:

* Parameter binding
* Conditional body injection
* Recursive expansion

---

## 🔬 11. COMPILER PIPELINE

1. **Lexing**
2. **Macro Expansion**
3. **Parsing (AST)**
4. **Semantic Checking**
5. **Intermediate Representation (.fir)**
6. **NASM Codegen**
7. **Assembly & Linking**

---

## ⚙️ 12. CODEGEN: NASM & STACK

* Functions use:

```nasm
push rbp
mov rbp, rsp
sub rsp, 32
```

* Variables allocated at `[rbp - offset]`
* Parameters from `[rbp + offset]`
* Return via `rax`

---

## 🔍 13. DEBUGGING & LOGGING

### `output/hello.log`

Includes:

* Source snapshot
* Token stream
* AST structure
* Semantic analysis results
* IR instructions
* NASM output references

---

## 🧬 14. ADVANCED OPTIMIZATION PATTERNS

* Inline returns
* Tail-call optimization
* Jump-threading for conditionals
* Register reuse heuristics
* Static inlining of macros
* Early constant folding in `.fir`

---

## 🗂 15. PROJECT ARCHITECTURE

```
HyperlaceCompiler/
├── src/
├── samples/
├── output/
├── run.sh / build.bat
├── Makefile
```

---

## 📚 APPENDIXES

### **A. BNF Grammar**

```ebnf
program        ::= { statement } ;
statement      ::= assignment | if | while | for | function | struct | return ;
expression     ::= term { ( '+' | '-' ) term } ;
term           ::= factor { ( '*' | '/' ) factor } ;
factor         ::= NUMBER | IDENT | '(' expression ')' ;
```

### **B. IR Table**

```txt
STORE x <- NUM(5)
ADD x, y
JUMP_IF_ZERO label
CALL sum, args
```

### **C. NASM Mapping**

| HL       | NASM          |
| -------- | ------------- |
| `+`      | `add`         |
| `-`      | `sub`         |
| `*`      | `imul`        |
| `==`     | `cmp + je`    |
| `Return` | `jmp .return` |

---

## 🧾 CHEAT SHEET

```hl
Init Person { name; age; }
p = Person();        # Struct init
p.name = "Alice";

Start double(x) {
    Return x * 2;
}

result = double(5);  # Function call

|inc| x = x + 1;     # Macro

x = (a > b) ? a : b; # Ternary

if (x < 10) {
    x += 1;
} else {
    x = 0;
}
```

---

