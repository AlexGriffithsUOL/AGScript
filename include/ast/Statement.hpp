#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Expression.hpp"
#include "Lexer.hpp"

class Statement {
public:
    virtual ~Statement() = default;
};

using StmtPtr = std::unique_ptr<Statement>;

class ExpressionStatement : public Statement {
public:
    ExprPtr expression;

    explicit ExpressionStatement(ExprPtr expression) : expression(std::move(expression)) {}
};

class VariableDeclaration : public Statement {
public:
    std::string name;
    ExprPtr initializer; // can be nullptr if no initializer

    VariableDeclaration(std::string name, ExprPtr initializer)
        : name(std::move(name)), initializer(std::move(initializer)) {}
};

class BlockStatement : public Statement {
public:
    std::vector<StmtPtr> statements;

    explicit BlockStatement(std::vector<StmtPtr> statements)
        : statements(std::move(statements)) {}
};

class IfStatement : public Statement {
public:
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch; // can be nullptr

    IfStatement(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch = nullptr)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
};

class WhileStatement : public Statement {
public:
    ExprPtr condition;
    StmtPtr body;

    WhileStatement(ExprPtr condition, StmtPtr body)
        : condition(std::move(condition)), body(std::move(body)) {}
};

class ForStatement : public Statement {
public:
    StmtPtr initializer; // variable decl, expr statement, or nullptr
    ExprPtr condition;   // can be nullptr
    ExprPtr increment;   // can be nullptr
    StmtPtr body;

    ForStatement(StmtPtr initializer, ExprPtr condition, ExprPtr increment, StmtPtr body)
        : initializer(std::move(initializer)), condition(std::move(condition)), increment(std::move(increment)), body(std::move(body)) {}
};

class ReturnStatement : public Statement {
public:
    ExprPtr value; // can be nullptr

    explicit ReturnStatement(ExprPtr value) : value(std::move(value)) {}
};

class FunctionDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::string> parameters;
    StmtPtr body;

    FunctionDeclaration(std::string name, std::vector<std::string> parameters, StmtPtr body)
        : name(std::move(name)), parameters(std::move(parameters)), body(std::move(body)) {}
};
