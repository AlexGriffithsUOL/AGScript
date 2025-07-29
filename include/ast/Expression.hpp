#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Lexer.hpp"

class Expression {
public:
    virtual ~Expression() = default;
};

using ExprPtr = std::unique_ptr<Expression>;

class LiteralExpression : public Expression {
public:
    Token literal;

    explicit LiteralExpression(const Token& literal) : literal(literal) {}
};

class VariableExpression : public Expression {
public:
    std::string name;

    explicit VariableExpression(std::string name) : name(std::move(name)) {}
};

class UnaryExpression : public Expression {
public:
    TokenType op;
    ExprPtr right;

    UnaryExpression(TokenType op, ExprPtr right) : op(op), right(std::move(right)) {}
};

class BinaryExpression : public Expression {
public:
    TokenType op;
    ExprPtr left;
    ExprPtr right;

    BinaryExpression(TokenType op, ExprPtr left, ExprPtr right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
};

class CallExpression : public Expression {
public:
    ExprPtr callee;
    std::vector<ExprPtr> arguments;

    CallExpression(ExprPtr callee, std::vector<ExprPtr> arguments)
        : callee(std::move(callee)), arguments(std::move(arguments)) {}
};
