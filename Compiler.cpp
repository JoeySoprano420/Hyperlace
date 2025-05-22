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
