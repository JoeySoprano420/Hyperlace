// hyperlace_compiler_core.cpp
// --- Hyperlace Compiler: Core Pipeline Bootstrapping ---
// Implements lexer, parser, AST, macro engine, semantic analyzer, IR emitter, and NASM backend.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cctype>
#include <stdexcept>
#include <set>

//--------------------------------------------------
// --- TOKEN DEFINITIONS ---
//--------------------------------------------------
enum class TokenType {
    Identifier, Number, String, Keyword, Symbol, Comment,
    Assign, ImmutableAssign, EndOfLine, EndOfFile
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

//--------------------------------------------------
// --- AST NODES ---
//--------------------------------------------------
class ASTNode {
public:
    virtual ~ASTNode() = default;
};

class Expression : public ASTNode {};
class Statement : public ASTNode {};

class Assignment : public Statement {
public:
    std::string name;
    std::shared_ptr<Expression> value;

    Assignment(const std::string& n, std::shared_ptr<Expression> v)
        : name(n), value(v) {}
};

class NumberExpr : public Expression {
public:
    std::string value;

    NumberExpr(const std::string& val) : value(val) {}
};

class IdentifierExpr : public Expression {
public:
    std::string name;

    IdentifierExpr(const std::string& n) : name(n) {}
};

// Function Definition
struct FunctionDef : public Statement {
    std::string name;
    std::vector<std::string> params;
    std::vector<std::shared_ptr<Statement>> body;
};

// If Statement
struct IfStatement : public Statement {
    std::shared_ptr<Expression> condition;
    std::vector<std::shared_ptr<Statement>> thenBranch;
    std::vector<std::shared_ptr<Statement>> elseBranch;
};

// Function Call
struct FunctionCall : public Expression {
    std::string name;
    std::vector<std::shared_ptr<Expression>> arguments;
};

// While loop
struct WhileLoop : public Statement {
    std::shared_ptr<Expression> condition;
    std::vector<std::shared_ptr<Statement>> body;
};

// For loop
struct ForLoop : public Statement {
    std::shared_ptr<Statement> initializer;
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> increment;
    std::vector<std::shared_ptr<Statement>> body;
};

// Function Call
struct FunctionCall : public Expression {
    std::string functionName;
    std::vector<std::shared_ptr<Expression>> arguments;
};

struct StructDef : public Statement {
    std::string name;
    std::vector<std::string> fields;
};

struct StructInit : public Expression {
    std::string structName;
};

struct FieldAccess : public Expression {
    std::shared_ptr<Expression> object;
    std::string field;
};

struct EnumDef : public Statement {
    std::string name;
    std::vector<std::string> variants;
};

//--------------------------------------------------
// --- PARSER IMPLEMENTATION ---
//--------------------------------------------------
class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

    std::vector<std::shared_ptr<Statement>> parse() {
        std::vector<std::shared_ptr<Statement>> statements;
        while (!isAtEnd()) {
            statements.push_back(parseStatement());
        }
        return statements;
    }

private:
    const std::vector<Token>& tokens;
    size_t current;

    bool isAtEnd() const {
        return peek().type == TokenType::EndOfFile;
    }

    const Token& peek() const {
        return tokens[current];
    }

    const Token& advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    const Token& previous() const {
        return tokens[current - 1];
    }

    bool match(TokenType type) {
        if (peek().type == type) {
            advance();
            return true;
        }
        return false;
    }

    std::shared_ptr<Statement> parseStatement() {
        if (peek().type == TokenType::Identifier && tokens[current + 1].type == TokenType::Assign) {
            return parseAssignment();
        }
        throw std::runtime_error("Unexpected statement");
    }

    std::shared_ptr<Statement> parseAssignment() {
        std::string name = peek().lexeme;
        advance(); // Identifier
        match(TokenType::Assign); // '='
        auto expr = parseExpression();
        match(TokenType::EndOfLine); // ';'
        return std::make_shared<Assignment>(name, expr);
    }

    std::shared_ptr<Expression> parseExpression() {
        if (match(TokenType::Number)) {
            return std::make_shared<NumberExpr>(previous().lexeme);
        } else if (match(TokenType::Identifier)) {
            return std::make_shared<IdentifierExpr>(previous().lexeme);
        }
        throw std::runtime_error("Invalid expression");
    }
};

std::shared_ptr<Statement> parseFunction() {
    advance(); // Skip 'Start'
    std::string name = expectIdentifier("Expected function name after Start");
    expect('(');
    std::vector<std::string> params;
    while (!match(')')) {
        params.push_back(expectIdentifier("Expected parameter name"));
        match(','); // optional comma
    }
    expect('{');
    std::vector<std::shared_ptr<Statement>> body;
    while (!check('}')) {
        body.push_back(parseStatement());
    }
    expect('}');
    return std::make_shared<FunctionDef>(FunctionDef{name, params, body});
}

std::shared_ptr<Statement> parseIfStatement() {
    advance(); // Skip 'if'
    expect('(');
    auto condition = parseExpression();
    expect(')');
    expect('{');
    std::vector<std::shared_ptr<Statement>> thenBranch;
    while (!check('}')) {
        thenBranch.push_back(parseStatement());
    }
    expect('}');
    std::vector<std::shared_ptr<Statement>> elseBranch;
    if (matchKeyword("else")) {
        expect('{');
        while (!check('}')) {
            elseBranch.push_back(parseStatement());
        }
        expect('}');
    }
    return std::make_shared<IfStatement>(IfStatement{condition, thenBranch, elseBranch});
}
std::shared_ptr<Statement> parseWhile() {
    advance(); // skip 'while'
    expect('(');
    auto condition = parseExpression();
    expect(')');
    expect('{');
    std::vector<std::shared_ptr<Statement>> body;
    while (!check('}')) {
        body.push_back(parseStatement());
    }
    expect('}');
    return std::make_shared<WhileLoop>(WhileLoop{condition, body});
}

std::shared_ptr<Statement> parseFor() {
    advance(); // skip 'for'
    expect('(');
    auto initializer = parseStatement();
    auto condition = parseExpression();
    expect(';');
    auto increment = parseStatement();
    expect(')');
    expect('{');
    std::vector<std::shared_ptr<Statement>> body;
    while (!check('}')) {
        body.push_back(parseStatement());
    }
    expect('}');
    return std::make_shared<ForLoop>(ForLoop{initializer, condition, increment, body});
}

std::shared_ptr<Expression> parseFunctionCall(const std::string& name) {
    advance(); // skip '('
    std::vector<std::shared_ptr<Expression>> args;
    while (!check(')')) {
        args.push_back(parseExpression());
        if (!match(')')) match(','); // handle comma-separated arguments
    }
    return std::make_shared<FunctionCall>(FunctionCall{name, args});
}

std::shared_ptr<Statement> parseStructDef() {
    advance(); // Skip 'Init'
    std::string name = expectIdentifier("Expected struct name.");
    expect('{');
    std::vector<std::string> fields;
    while (!match('}')) {
        fields.push_back(expectIdentifier("Expected field name."));
        match(';');
    }
    return std::make_shared<StructDef>(StructDef{name, fields});
}

std::shared_ptr<Expression> parseStructInit(const std::string& name) {
    expect('('); expect(')'); // e.g. Person()
    return std::make_shared<StructInit>(StructInit{name});
}

std::shared_ptr<Expression> parseFieldAccess(std::shared_ptr<Expression> obj) {
    expect('.'); // p.name
    std::string field = expectIdentifier("Expected field name.");
    return std::make_shared<FieldAccess>(FieldAccess{obj, field});
}

enum Precedence {
    PREC_LOWEST,
    PREC_ASSIGN,   // =
    PREC_COND,     // ?:
    PREC_SUM,      // + -
    PREC_PRODUCT,  // * /
    PREC_PREFIX,   // -x
    PREC_CALL,     // ()
    PREC_PRIMARY
};

if (match(TokenType::PlusEq)) {
    auto value = parseExpression();
    return std::make_shared<Assignment>(varName,
        std::make_shared<BinaryExpr>("+", readVar(varName), value));
}

//--------------------------------------------------
// --- MACRO ENGINE (C.I.A.M.S.) ---
//--------------------------------------------------
class MacroExpander {
public:
    void define(const std::string& name, const std::string& replacement) {
        macros[name] = replacement;
    }

    std::string expand(const std::string& input) {
        std::string result;
        std::istringstream stream(input);
        std::string word;
        while (stream >> word) {
            if (macros.find(word) != macros.end()) {
                result += macros[word] + " ";
            } else {
                result += word + " ";
            }
        }
        return result;
    }

    void loadDefaults() {
        define("|inc|", "x = x + 1;");
        define("|dec|", "x = x - 1;");
        define("|reset|", "x = 0;");
    }

private:
    std::unordered_map<std::string, std::string> macros;
};

//--------------------------------------------------
// --- SEMANTIC ANALYZER ---
//--------------------------------------------------
class SemanticAnalyzer {
public:
    void analyze(const std::vector<std::shared_ptr<Statement>>& statements) {
        declared.clear();
        for (const auto& stmt : statements) {
            if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
                analyzeAssignment(assign);
            }
        }
    }

private:
    std::set<std::string> declared;

    void analyzeAssignment(const std::shared_ptr<Assignment>& stmt) {
        declared.insert(stmt->name);

        if (auto idExpr = std::dynamic_pointer_cast<IdentifierExpr>(stmt->value)) {
            if (declared.find(idExpr->name) == declared.end()) {
                throw std::runtime_error("Semantic Error: Use of undeclared variable '" + idExpr->name + "'");
            }
        }
    }
};

bool inFunction = false;

for (const auto& stmt : statements) {
    if (auto fn = std::dynamic_pointer_cast<FunctionDef>(stmt)) {
        inFunction = true;
        analyze(fn->body);  // recursively walk inside
        inFunction = false;
    }
    else if (auto ret = std::dynamic_pointer_cast<ReturnStatement>(stmt)) {
        if (!inFunction)
            throw std::runtime_error("Return statement used outside a function.");
    }
}

//--------------------------------------------------
// --- INTERMEDIATE REPRESENTATION EMITTER ---
//--------------------------------------------------
class IREmitter {
public:
    void emit(const std::vector<std::shared_ptr<Statement>>& statements, const std::string& outPath) {
        std::ofstream out(outPath);
        if (!out) throw std::runtime_error("Failed to write IR file.");

        for (const auto& stmt : statements) {
            if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
                out << "STORE " << assign->name << " <- ";
                if (auto num = std::dynamic_pointer_cast<NumberExpr>(assign->value)) {
                    out << "NUM(" << num->value << ")\n";
                } else if (auto id = std::dynamic_pointer_cast<IdentifierExpr>(assign->value)) {
                    out << "REF(" << id->name << ")\n";
                }
            }
        }
    }
};

//--------------------------------------------------
// --- NASM CODE GENERATOR ---
//--------------------------------------------------
class NASMGenerator {
public:
    void generate(const std::vector<std::shared_ptr<Statement>>& statements, const std::string& outputPath) {
        std::ofstream out(outputPath);
        if (!out) throw std::runtime_error("Failed to write ASM file.");

        out << "section .data\n";
        for (const auto& stmt : statements) {
            if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
                out << assign->name << " dq 0\n";
            }
        }

        out << "\nsection .text\n global _start\n_start:\n";
        for (const auto& stmt : statements) {
            if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
                if (auto num = std::dynamic_pointer_cast<NumberExpr>(assign->value)) {
                    out << "    mov rax, " << num->value << "\n";
                    out << "    mov [" << assign->name << "], rax\n";
                } else if (auto id = std::dynamic_pointer_cast<IdentifierExpr>(assign->value)) {
                    out << "    mov rax, [" << id->name << "]\n";
                    out << "    mov [" << assign->name << "], rax\n";
                }
            }
        }
        out << "    mov rax, 60\n    xor rdi, rdi\n    syscall\n";
    }
};

//--------------------------------------------------
// --- MAIN: FULL COMPILER TEST HARNESS ---
//--------------------------------------------------
int main() {
    std::ifstream file("samples/hello.hl");
    if (!file) {
        std::cerr << "Failed to open sample file." << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    MacroExpander expander;
    expander.loadDefaults();

    std::string expanded = expander.expand(buffer.str());
    Lexer lexer(expanded);

    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    auto statements = parser.parse();

    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(statements);
        std::cout << "Semantic analysis successful.\n";
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    IREmitter ir;
    ir.emit(statements, "output/hello.fir");
    std::cout << "IR written to output/hello.fir\n";

    NASMGenerator nasm;
    nasm.generate(statements, "output/hello.asm");
    std::cout << "NASM assembly written to output/hello.asm\n";

    std::cout << "Parsed " << statements.size() << " statement(s).\n";
    for (const auto& stmt : statements) {
        if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
            std::cout << "Assignment to: " << assign->name << "\n";
        }
    }
    return 0;
}

#include <chrono>
int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::ifstream file("samples/hello.hl");
    if (!file) {
        std::cerr << "Failed to open sample file." << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string raw_input = buffer.str();

    MacroExpander expander;
    expander.loadDefaults();

    std::string expanded = expander.expand(raw_input);
    Lexer lexer(expanded);

    std::ofstream log("output/hello.log");
    if (!log) {
        std::cerr << "Failed to open debug log." << std::endl;
        return 1;
    }

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    log << "Hyperlace Compiler Debug Log
";
    log << "Timestamp: " << std::ctime(&now);
    log << "----------------------------------------

";

    log << "[Source Code]
" << raw_input << "

";
    log << "[Expanded Code]
" << expanded << "

";

    log << "[Tokens]\n";
    for (const auto& token : tokens) {
        log << std::setw(4) << token.line << ":" << std::setw(2) << token.column << "\t"
            << static_cast<int>(token.type) << "\t" << token.lexeme << "\n";
    }

    Parser parser(tokens);
    auto statements = parser.parse();
    log << "\n[AST]\n";
    for (const auto& stmt : statements) {
        if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
            log << "Assign to " << assign->name << " <- ";
            if (auto num = std::dynamic_pointer_cast<NumberExpr>(assign->value)) {
                log << "NUM(" << num->value << ")\n";
            } else if (auto id = std::dynamic_pointer_cast<IdentifierExpr>(assign->value)) {
                log << "REF(" << id->name << ")\n";
            }
        }
    }

    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(statements);
        std::cout << "Semantic analysis successful.\n";
        log << "\n[Semantic] Success\n";
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        log << "\n[Semantic Error] " << ex.what() << "\n";
        return 1;
    }

    IREmitter ir;
    ir.emit(statements, "output/hello.fir");
    log << "\n[IR] Emitted to hello.fir\n";

    NASMGenerator nasm;
    nasm.generate(statements, "output/hello.asm");
    log << "[ASM] Emitted to hello.asm\n";

    std::cout << "Parsed " << statements.size() << " statement(s).\n";
    for (const auto& stmt : statements) {
        if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
            std::cout << "Assignment to: " << assign->name << "\n";
        }
    }

        log << "
[Statistics]
";
    log << "Total Statements: " << statements.size() << "
";

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    log << "Compile Time: " << duration << "ms
";

    log << "
[Status] Compilation Completed.
";
    log.close();
    return 0;
}

// hyperlace_compiler_core.cpp
// --- Hyperlace Compiler: Core Pipeline Bootstrapping ---
// Implements lexer, parser, AST, macro engine, semantic analyzer, IR emitter, NASM backend, and debug logger.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cctype>
#include <stdexcept>
#include <set>
#include <iomanip> // For formatting debug output

// ... [rest of code remains unchanged until main() below] ...

//--------------------------------------------------
// --- AST XML EMITTER ---
//--------------------------------------------------
class ASTXMLWriter {
public:
    void emit(const std::vector<std::shared_ptr<Statement>>& statements, const std::string& path) {
        std::ofstream out(path);
        if (!out) throw std::runtime_error("Failed to open .ast file");

        out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
";
        out << "<program>
";
        for (const auto& stmt : statements) {
            if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
                out << "  <assignment var=\"" << assign->name << "\">";
                if (auto num = std::dynamic_pointer_cast<NumberExpr>(assign->value)) {
                    out << "<number>" << num->value << "</number>";
                } else if (auto id = std::dynamic_pointer_cast<IdentifierExpr>(assign->value)) {
                    out << "<identifier>" << id->name << "</identifier>";
                }
                out << "</assignment>
";
            }
        }
        out << "</program>
";
    }
};

void writeASTToXML(const std::vector<std::shared_ptr<Statement>>& statements, std::ostream& out) {
    out << "<Program>\n";
    for (const auto& stmt : statements) {
        if (auto fn = std::dynamic_pointer_cast<FunctionDef>(stmt)) {
            out << "  <Function name=\"" << fn->name << "\">\n";
            for (auto& param : fn->params)
                out << "    <Param>" << param << "</Param>\n";
            out << "    <Body>\n";
            writeASTToXML(fn->body, out);
            out << "    </Body>\n  </Function>\n";
        }
        else if (auto ifs = std::dynamic_pointer_cast<IfStatement>(stmt)) {
            out << "  <If>\n";
            out << "    <Condition/>\n"; // implement this properly
            out << "    <Then>\n";
            writeASTToXML(ifs->thenBranch, out);
            out << "    </Then>\n";
            if (!ifs->elseBranch.empty()) {
                out << "    <Else>\n";
                writeASTToXML(ifs->elseBranch, out);
                out << "    </Else>\n";
            }
            out << "  </If>\n";
        }
        else if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
            out << "  <Assignment>\n";
            out << "    <Target>" << assign->name << "</Target>\n";
            if (auto num = std::dynamic_pointer_cast<NumberExpr>(assign->value)) {
                out << "    <Value type=\"Number\">" << num->value << "</Value>\n";
            } else if (auto id = std::dynamic_pointer_cast<IdentifierExpr>(assign->value)) {
                out << "    <Value type=\"Identifier\">" << id->name << "</Value>\n";
            }
            out << "  </Assignment>\n";
        }
    }
    out << "</Program>\n";
}

else if (auto w = std::dynamic_pointer_cast<WhileLoop>(stmt)) {
    out << "  <While>\n";
    out << "    <Condition/>\n"; // TODO: render condition
    out << "    <Body>\n";
    writeASTToXML(w->body, out);
    out << "    </Body>\n  </While>\n";
}
else if (auto f = std::dynamic_pointer_cast<ForLoop>(stmt)) {
    out << "  <For>\n";
    out << "    <Initializer/>\n"; // TODO: render init
    out << "    <Condition/>\n";  // TODO: render condition
    out << "    <Increment/>\n";  // TODO: render increment
    out << "    <Body>\n";
    writeASTToXML(f->body, out);
    out << "    </Body>\n  </For>\n";
}
else if (auto fc = std::dynamic_pointer_cast<FunctionCall>(stmt)) {
    out << "  <FunctionCall name=\"" << fc->functionName << "\">\n";
    for (auto& arg : fc->arguments)
        out << "    <Arg/>\n"; // TODO: render each argument
    out << "  </FunctionCall>\n";
}

struct ReturnStatement : public Statement {
    std::shared_ptr<Expression> returnValue; // optional
};

struct TernaryExpr : public Expression {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Expression> thenExpr;
    std::shared_ptr<Expression> elseExpr;
};

//--------------------------------------------------
// --- MAIN: FULL COMPILER TEST HARNESS ---
//--------------------------------------------------
#include <chrono>
int main() {
    auto start_time = std::chrono::high_resolution_clock::now();

    std::ifstream file("samples/hello.hl");
    if (!file) {
        std::cerr << "Failed to open sample file." << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string raw_input = buffer.str();

    MacroExpander expander;
    expander.loadDefaults();

    std::string expanded = expander.expand(raw_input);
    Lexer lexer(expanded);

    std::ofstream log("output/hello.log");
    if (!log) {
        std::cerr << "Failed to open debug log." << std::endl;
        return 1;
    }

    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    log << "Hyperlace Compiler Debug Log
";
    log << "Timestamp: " << std::ctime(&now);
    log << "----------------------------------------

";

    log << "[Source Code]
" << raw_input << "

";
    log << "[Expanded Code]
" << expanded << "

";

    log << "[Tokens]\n";
    for (const auto& token : tokens) {
        log << std::setw(4) << token.line << ":" << std::setw(2) << token.column << "\t"
            << static_cast<int>(token.type) << "\t" << token.lexeme << "\n";
    }

    Parser parser(tokens);
    auto statements = parser.parse();
    log << "\n[AST]\n";
    for (const auto& stmt : statements) {
        if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
            log << "Assign to " << assign->name << " <- ";
            if (auto num = std::dynamic_pointer_cast<NumberExpr>(assign->value)) {
                log << "NUM(" << num->value << ")\n";
            } else if (auto id = std::dynamic_pointer_cast<IdentifierExpr>(assign->value)) {
                log << "REF(" << id->name << ")\n";
            }
        }
    }

    SemanticAnalyzer analyzer;
    try {
        analyzer.analyze(statements);
        std::cout << "Semantic analysis successful.\n";
        log << "\n[Semantic] Success\n";
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        log << "\n[Semantic Error] " << ex.what() << "\n";
        return 1;
    }

    IREmitter ir;
    ir.emit(statements, "output/hello.fir");
    log << "\n[IR] Emitted to hello.fir\n";

    NASMGenerator nasm;
    nasm.generate(statements, "output/hello.asm");
    log << "[ASM] Emitted to hello.asm
";

    ASTXMLWriter astWriter;
    astWriter.emit(statements, "output/hello.ast");
    log << "[AST] XML written to output/hello.ast
";

    std::cout << "Parsed " << statements.size() << " statement(s).\n";
    for (const auto& stmt : statements) {
        if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
            std::cout << "Assignment to: " << assign->name << "\n";
        }
    }

        log << "
[Statistics]
";
    log << "Total Statements: " << statements.size() << "
";

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    log << "Compile Time: " << duration << "ms
";

    log << "
[Status] Compilation Completed.
";
    log.close();
    return 0;
}


# Emit Return Value (If any)

if (auto ret = std::dynamic_pointer_cast<ReturnStatement>(stmt)) {
    if (ret->returnValue) {
        // emit code to evaluate expression into RAX
        // e.g. mov rax, value
    }
    out << "    jmp .return\n";
}
