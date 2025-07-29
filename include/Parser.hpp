#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Lexer.hpp"  // Assumes you have a Token struct/class with TokenType, lexeme, etc.

class Expression;
class Statement;

// Base classes
class Expression {
public:
    virtual ~Expression() = default;
};

class Statement {
public:
    virtual ~Statement() = default;
};

using ExprPtr = std::unique_ptr<Expression>;
using StmtPtr = std::unique_ptr<Statement>;

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<StmtPtr> parse();

private:
    // Helpers
    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type) const;
    Token advance();
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    void error(const Token& token, const std::string& message);
    void synchronize();

    // Parsing rules (non-terminals)
    std::vector<StmtPtr> program();
    StmtPtr declaration();
    StmtPtr function_decl();
    StmtPtr variable_decl();
    StmtPtr statement();
    StmtPtr expression_statement();
    StmtPtr if_statement();
    StmtPtr while_statement();
    StmtPtr for_statement();
    StmtPtr return_statement();
    StmtPtr block();

    ExprPtr expression();
    ExprPtr assignment();
    ExprPtr logical_or();
    ExprPtr logical_and();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr addition();
    ExprPtr multiplication();
    ExprPtr unary();
    ExprPtr primary();
    ExprPtr function_call(const ExprPtr& callee);

    std::vector<ExprPtr> parameter_list();
    std::vector<ExprPtr> argument_list();

    // Tokens
    const std::vector<Token>& tokens;
    size_t current;
};
