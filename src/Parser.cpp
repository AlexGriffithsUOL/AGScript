#include "Parser.hpp"
#include <stdexcept>
#include <iostream>
#include "include/ast/Expression.hpp"
#include "include/ast/Statement.hpp"

// Forward declarations for AST node classes omitted here for brevity.
// Assume you will define appropriate subclasses of Expression and Statement
// for each construct like FunctionDeclStmt, VariableDeclStmt, BinaryExpr, etc.

// --- Parser constructor ---
Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

// --- Parse entry point ---
std::vector<StmtPtr> Parser::parse() {
    return program();
}

// --- Helpers ---
bool Parser::match(std::initializer_list<TokenType> types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

void Parser::error(const Token& token, const std::string& message) {
    std::cerr << "[Parser Error] at token '" << token.value << "': " << message << "\n";
    // In real usage, throw or handle error appropriately
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMI_COLON) return;

        switch (peek().type) {
            case TokenType::FUNCTION:
            case TokenType::LET:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::RETURN:
                return;
            default:
                advance();
        }
    }
}

// --- Grammar rules ---

// program ::= { declaration } ;
std::vector<StmtPtr> Parser::program() {
    std::vector<StmtPtr> declarations;
    while (!isAtEnd()) {
        try {
            declarations.push_back(declaration());
        } catch (...) {
            synchronize();
        }
    }
    return declarations;
}

// declaration ::= function_decl | variable_decl ;
StmtPtr Parser::declaration() {
    if (match({TokenType::FUNCTION})) return function_decl();
    if (match({TokenType::LET})) return variable_decl();

    // Fall back to statement
    return statement();
}

// function_decl ::= FUNCTION IDENTIFIER LEFT_PARENTHESIS [ parameter_list ] RIGHT_PARENTHESIS block ;
StmtPtr Parser::function_decl() {
    // Expect IDENTIFIER
    if (!check(TokenType::IDENTIFIER)) {
        error(peek(), "Expected function name after 'function'");
        throw std::runtime_error("Parse error");
    }
    Token name = advance();

    if (!match({TokenType::LEFT_PARENTHESIS})) {
        error(peek(), "Expected '(' after function name");
        throw std::runtime_error("Parse error");
    }

    std::vector<ExprPtr> params;
    if (!check(TokenType::RIGHT_PARENTHESIS)) {
        // parse parameters as identifiers wrapped in expressions or a special Param type
        // For simplicity, treat as variable expressions here:
        do {
            if (!check(TokenType::IDENTIFIER)) {
                error(peek(), "Expected parameter name");
                throw std::runtime_error("Parse error");
            }
            Token paramName = advance();
            // You might want a specific Param node, but we'll store as VariableExpression for now
            // TODO: create Param node if needed
            // Just push some Expression wrapping paramName.lexeme or so.
            // For now, leave empty or a placeholder.
        } while (match({TokenType::COMMA}));
    }

    if (!match({TokenType::RIGHT_PARENTHESIS})) {
        error(peek(), "Expected ')' after function parameters");
        throw std::runtime_error("Parse error");
    }

    StmtPtr body = block();

    // TODO: return new FunctionDeclaration node with name, params, and body
    return nullptr; // placeholder
}

// variable_decl ::= LET IDENTIFIER [ ASSIGN expression ] SEMI_COLON ;
StmtPtr Parser::variable_decl() {
    if (!check(TokenType::IDENTIFIER)) {
        error(peek(), "Expected variable name after 'let'");
        throw std::runtime_error("Parse error");
    }
    Token name = advance();

    ExprPtr initializer = nullptr;
    if (match({TokenType::ASSIGN})) {
        initializer = expression();
    }

    if (!match({TokenType::SEMI_COLON})) {
        error(peek(), "Expected ';' after variable declaration");
        throw std::runtime_error("Parse error");
    }

    // TODO: return new VariableDeclaration node with name and initializer
    return nullptr; // placeholder
}

// block ::= LEFT_BRACE { statement } RIGHT_BRACE ;
StmtPtr Parser::block() {
    if (!match({TokenType::LEFT_BRACE})) {
        error(peek(), "Expected '{' to start block");
        throw std::runtime_error("Parse error");
    }

    std::vector<StmtPtr> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(statement());
    }

    if (!match({TokenType::RIGHT_BRACE})) {
        error(peek(), "Expected '}' after block");
        throw std::runtime_error("Parse error");
    }

    // TODO: return BlockStatement node containing statements
    return nullptr; // placeholder
}

// statement ::= expression_statement | if_statement | while_statement | return_statement | for_statement | variable_decl | block | SEMI_COLON ;
StmtPtr Parser::statement() {
    if (match({TokenType::IF})) return if_statement();
    if (match({TokenType::WHILE})) return while_statement();
    if (match({TokenType::RETURN})) return return_statement();
    if (match({TokenType::FOR})) return for_statement();
    if (match({TokenType::LET})) return variable_decl();
    if (match({TokenType::LEFT_BRACE})) return block();

    if (match({TokenType::SEMI_COLON})) {
        // Empty statement
        // TODO: return EmptyStatement node or nullptr
        return nullptr;
    }

    return expression_statement();
}

// expression_statement ::= expression SEMI_COLON ;
StmtPtr Parser::expression_statement() {
    ExprPtr expr = expression();
    if (!match({TokenType::SEMI_COLON})) {
        error(peek(), "Expected ';' after expression");
        throw std::runtime_error("Parse error");
    }
    // TODO: return ExpressionStatement node wrapping expr
    return nullptr; // placeholder
}

// if_statement ::= IF LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement [ ELSE statement ] ;
StmtPtr Parser::if_statement() {
    if (!match({TokenType::LEFT_PARENTHESIS})) {
        error(peek(), "Expected '(' after 'if'");
        throw std::runtime_error("Parse error");
    }

    ExprPtr condition = expression();

    if (!match({TokenType::RIGHT_PARENTHESIS})) {
        error(peek(), "Expected ')' after if condition");
        throw std::runtime_error("Parse error");
    }

    StmtPtr thenBranch = statement();
    StmtPtr elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }

    // TODO: return IfStatement node with condition, thenBranch, elseBranch
    return nullptr; // placeholder
}

// while_statement ::= WHILE LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement ;
StmtPtr Parser::while_statement() {
    if (!match({TokenType::LEFT_PARENTHESIS})) {
        error(peek(), "Expected '(' after 'while'");
        throw std::runtime_error("Parse error");
    }

    ExprPtr condition = expression();

    if (!match({TokenType::RIGHT_PARENTHESIS})) {
        error(peek(), "Expected ')' after while condition");
        throw std::runtime_error("Parse error");
    }

    StmtPtr body = statement();

    // TODO: return WhileStatement node with condition and body
    return nullptr; // placeholder
}

// for_statement ::= FOR LEFT_PARENTHESIS [ variable_decl | expression_statement | SEMI_COLON ]
//                    expression SEMI_COLON
//                    [ expression ]
//                  RIGHT_PARENTHESIS statement ;
StmtPtr Parser::for_statement() {
    if (!match({TokenType::LEFT_PARENTHESIS})) {
        error(peek(), "Expected '(' after 'for'");
        throw std::runtime_error("Parse error");
    }

    StmtPtr initializer = nullptr;
    if (match({TokenType::SEMI_COLON})) {
        // empty initializer
    } else if (match({TokenType::LET})) {
        initializer = variable_decl();
    } else {
        initializer = expression_statement();
    }

    ExprPtr condition = nullptr;
    if (!check(TokenType::SEMI_COLON)) {
        condition = expression();
    }

    if (!match({TokenType::SEMI_COLON})) {
        error(peek(), "Expected ';' after loop condition");
        throw std::runtime_error("Parse error");
    }

    ExprPtr increment = nullptr;
    if (!check(TokenType::RIGHT_PARENTHESIS)) {
        increment = expression();
    }

    if (!match({TokenType::RIGHT_PARENTHESIS})) {
        error(peek(), "Expected ')' after for clauses");
        throw std::runtime_error("Parse error");
    }

    StmtPtr body = statement();

    // TODO: return ForStatement node with initializer, condition, increment, and body
    return nullptr; // placeholder
}

// return_statement ::= RETURN [ expression ] SEMI_COLON ;
StmtPtr Parser::return_statement() {
    ExprPtr value = nullptr;
    if (!check(TokenType::SEMI_COLON)) {
        value = expression();
    }
    if (!match({TokenType::SEMI_COLON})) {
        error(peek(), "Expected ';' after return statement");
        throw std::runtime_error("Parse error");
    }

    // TODO: return ReturnStatement node with value
    return nullptr; // placeholder
}

// expression ::= assignment ;
ExprPtr Parser::expression() {
    return assignment();
}

// assignment ::= IDENTIFIER ASSIGN assignment | logical_or ;
ExprPtr Parser::assignment() {
    ExprPtr expr = logical_or();

    if (match({TokenType::ASSIGN})) {
        Token equals = previous();

        ExprPtr value = assignment();

        // Left side must be an identifier
        // TODO: add code to check expr is VariableExpression and create Assignment node
        // For now, just throw
        error(equals, "Invalid assignment target.");
        throw std::runtime_error("Parse error");
    }

    return expr;
}

// logical_or ::= logical_and { OR logical_and } ;
ExprPtr Parser::logical_or() {
    ExprPtr expr = logical_and();

    while (match({TokenType::OR})) {
        Token op = previous();
        ExprPtr right = logical_and();
        // TODO: return BinaryExpression node with OR op
    }

    return expr;
}

// logical_and ::= equality { AND equality } ;
ExprPtr Parser::logical_and() {
    ExprPtr expr = equality();

    while (match({TokenType::AND})) {
        Token op = previous();
        ExprPtr right = equality();
        // TODO: return BinaryExpression node with AND op
    }

    return expr;
}

// equality ::= comparison { (EQUAL | NOT_EQUAL) comparison } ;
ExprPtr Parser::equality() {
    ExprPtr expr = comparison();

    while (match({TokenType::EQUAL, TokenType::NOT_EQUAL})) {
        Token op = previous();
        ExprPtr right = comparison();
        // TODO: return BinaryExpression node with op
    }

    return expr;
}

// comparison ::= addition { (LESS_THAN | LESS_THAN_OR_EQUAL | GREATER_THAN | GREATER_THAN_OR_EQUAL) addition } ;
ExprPtr Parser::comparison() {
    ExprPtr expr = addition();

    while (match({TokenType::LESS_THAN, TokenType::LESS_THAN_OR_EQUAL,
                  TokenType::GREATER_THAN, TokenType::GREATER_THAN_OR_EQUAL})) {
        Token op = previous();
        ExprPtr right = addition();
        // TODO: return BinaryExpression node with op
        // For now, placeholder:
        expr = std::make_unique<BinaryExpression>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}

// addition ::= multiplication { (ADD | SUBTRACT) multiplication } ;
ExprPtr Parser::addition() {
    ExprPtr expr = multiplication();

    while (match({TokenType::ADD, TokenType::SUBTRACT})) {
        Token op = previous();
        ExprPtr right = multiplication();
        expr = std::make_unique<BinaryExpression>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}

// multiplication ::= unary { (MULTIPLY | DIVIDE) unary } ;
ExprPtr Parser::multiplication() {
    ExprPtr expr = unary();

    while (match({TokenType::MULTIPLY, TokenType::DIVIDE})) {
        Token op = previous();
        ExprPtr right = unary();
        expr = std::make_unique<BinaryExpression>(op.type, std::move(expr), std::move(right));
    }

    return expr;
}

// unary ::= (NOT | SUBTRACT) unary | primary ;
ExprPtr Parser::unary() {
    if (match({TokenType::NOT, TokenType::SUBTRACT})) {
        Token op = previous();
        ExprPtr right = unary();
        return std::make_unique<UnaryExpression>(op.type, std::move(right));
    }
    return primary();
}

// primary ::= INT_LITERAL | FLOAT_LITERAL | STRING_LITERAL | BOOLEAN_LITERAL | NULL_LITERAL | IDENTIFIER | FUNCTION_CALL | LEFT_PARENTHESIS expression RIGHT_PARENTHESIS ;
ExprPtr Parser::primary() {
    if (match({TokenType::INT_LITERAL, TokenType::FLOAT_LITERAL,
               TokenType::STRING_LITERAL, TokenType::BOOLEAN_LITERAL,
               TokenType::NULL_LITERAL})) {
        Token literal = previous();
        return std::make_unique<LiteralExpression>(literal);
    }

    if (match({TokenType::IDENTIFIER})) {
        Token id = previous();
        // Check for function call: IDENTIFIER LEFT_PARENTHESIS ...
        if (check(TokenType::LEFT_PARENTHESIS)) {
            return function_call(std::make_unique<VariableExpression>(id.value));
        }
        return std::make_unique<VariableExpression>(id.value);
    }

    if (match({TokenType::LEFT_PARENTHESIS})) {
        ExprPtr expr = expression();
        if (!match({TokenType::RIGHT_PARENTHESIS})) {
            error(peek(), "Expected ')' after expression.");
            throw std::runtime_error("Parse error");
        }
        return expr;
    }

    error(peek(), "Expected expression.");
    throw std::runtime_error("Parse error");
}

// FUNCTION_CALL ::= IDENTIFIER LEFT_PARENTHESIS [ argument_list ] RIGHT_PARENTHESIS ;
ExprPtr Parser::function_call(const ExprPtr& callee) {
    // We assume callee is a VariableExpression for the function name

    if (!match({TokenType::LEFT_PARENTHESIS})) {
        error(peek(), "Expected '(' after function name");
        throw std::runtime_error("Parse error");
    }

    std::vector<ExprPtr> args;
    if (!check(TokenType::RIGHT_PARENTHESIS)) {
        args = argument_list();
    }

    if (!match({TokenType::RIGHT_PARENTHESIS})) {
        error(peek(), "Expected ')' after arguments");
        throw std::runtime_error("Parse error");
    }

    return std::make_unique<CallExpression>(callee, std::move(args));
}

// argument_list ::= expression { COMMA expression } ;
std::vector<ExprPtr> Parser::argument_list() {
    std::vector<ExprPtr> args;
    do {
        args.push_back(expression());
    } while (match({TokenType::COMMA}));
    return args;
}

// parameter_list ::= IDENTIFIER { COMMA IDENTIFIER } ;
std::vector<ExprPtr> Parser::parameter_list() {
    std::vector<ExprPtr> params;
    do {
        if (!check(TokenType::IDENTIFIER)) {
            error(peek(), "Expected parameter name");
            throw std::runtime_error("Parse error");
        }
        Token param = advance();
        params.push_back(std::make_unique<VariableExpression>(param.value));
    } while (match({TokenType::COMMA}));
    return params;
}
