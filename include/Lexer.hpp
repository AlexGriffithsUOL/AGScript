#ifndef TokenHeader
#define TokenHeader
#endif

#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <unordered_map>

enum class TokenType {
    IDENTIFIER,
    IF,
    ADD,
    MULTIPLY,
    SUBTRACT,
    DIVIDE,
    EQUAL,
    FUNCTION,
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    NOT_EQUAL,
    LESS_THAN,
    LESS_THAN_OR_EQUAL,
    GREATER_THAN,
    GREATER_THAN_OR_EQUAL,
    AND,
    OR,
    NOT,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    LEFT_BRACE,
    RIGHT_BRACE,
    DOT,
    NEW_LINE,
    COMMENT,
    END_OF_FILE,
    COMMA,
    SEMI_COLON,
    INT_LITERAL,
    STRING_LITERAL,
    BOOLEAN_LITERAL,
    FLOAT_LITERAL,
    NULL_LITERAL,
    ASSIGN,
    KEYWORD,
    ELSE,
    RETURN,
    WHILE,
    IN,
    FOR, 
    LET,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
};

class Lexer {
    public:
        Lexer(const std::string& source): source(source), position(0), line(1), column(1) {}
        
        Token getNextToken() {
        skipWhitespace();
        
        if (position >= source.size()) {
            return makeToken(TokenType::END_OF_FILE, "");
        };
            
        char current = source[position];

        if (current == 'f' or current == 't') {
            return lexBoolean();
        }

        // Identifiers: [a-zA-Z_][a-zA-Z0-9_]*
        if (std::isalpha(current) || current == '_') {
            return lexBoolean();
        };

        // Integer literals: [0-9]+
        if (std::isdigit(current)) {
            return lexNumber();
        };

        if (current == '\"') {
            return lexString();
        };


        // Operators and punctuation
        switch (current) {
            case '+':
                advance();
                return makeToken(TokenType::ADD, "+");
            case '-':
                advance();
                return makeToken(TokenType::SUBTRACT, "-");
            case '=':
                return lexAssignOrEqual();
            case '(':
                advance();
                return makeToken(TokenType::LEFT_PARENTHESIS, "(");
            case ')':
                advance();
                return makeToken(TokenType::RIGHT_PARENTHESIS, ")");
            case ';':
                advance();
                return makeToken(TokenType::SEMI_COLON, ";");
            case '{':
                advance();
                return makeToken(TokenType::LEFT_BRACKET, "{");
            case '}':
                advance();
                return makeToken(TokenType::RIGHT_BRACKET, "}");
            case '[':
                advance();
                return makeToken(TokenType::LEFT_BRACE, "]");
            case ']':
                advance();
                return makeToken(TokenType::RIGHT_BRACE, "]");
            case '.':
                advance();
                return makeToken(TokenType::DOT, ".");
            case '"':
                advance();
                return makeToken(TokenType::STRING_LITERAL, "\"");
            case ',':
                advance();
                return makeToken(TokenType::COMMA , ",");
            case '!':
                advance();
                return makeToken(TokenType::NOT , "!");
            default:
                advance();
                return makeToken(TokenType::UNKNOWN, std::string(1, current));
        };
    };

    private:
        const std::string& source;
        size_t position;
        int line;
        int column;

        void advance() {
            if (source[position] == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            position++;
        };

        void skipWhitespace() {
            while (position < source.size() && std::isspace(source[position])) {
                advance();
            };
        };

        Token makeToken(TokenType type, const std::string& value) {
            return Token{type, value, line, column - (int)value.size()};
        };

        Token lexIdentifier() {
            size_t start = position;
            int startCol = column;
            while (position < source.size() && (std::isalnum(source[position]) || source[position] == '_')) {
                advance();
            }
            return Token{TokenType::IDENTIFIER, source.substr(start, position - start), line, startCol};
        }

        Token lexString() {
            size_t start = position;      // Position of the opening quote
            int startColumn = column;
            
            advance();  // skip the opening quote '"'
            
            std::string strContent;

            while (position < source.size()) {
                char current = source[position];
                
                if (current == '"') {
                    advance();
                    break;
                }
                
                if (current == '\\') {
                    advance();
                    if (position >= source.size()) break;
                    
                    char nextChar = source[position];
                    if (nextChar == '"' || nextChar == '\\') {
                        strContent.push_back(nextChar);
                    } else {
                        strContent.push_back('\\');
                        strContent.push_back(nextChar);
                    }
                    advance();
                } else {
                    strContent.push_back(current);
                    advance();
                };
            };

            return Token{TokenType::STRING_LITERAL, strContent, line, startColumn};
        };

        Token lexBoolean() {
            size_t start = position;
            int startColumn = column;

            std::string ident;

            while (position < source.size() && std::isalpha(source[position])) {
                ident.push_back(source[position]);
                advance();
            }

            static std::unordered_map<std::string, TokenType> keywords = {
                {"return", TokenType::RETURN},
                {"function", TokenType::FUNCTION},
                {"if", TokenType::IF},
                {"else", TokenType::ELSE},
                {"true", TokenType::BOOLEAN_LITERAL},
                {"false", TokenType::BOOLEAN_LITERAL},
                {"null", TokenType::NULL_LITERAL},
                {"and", TokenType::AND},
                {"or", TokenType::OR},
                {"not", TokenType::NOT},
                {"while", TokenType::WHILE},
                {"for", TokenType::FOR},
                {"in", TokenType::IN},
                {"let", TokenType::LET},
            };

            auto it = keywords.find(ident);

            // std::cout << "lexed identifier: " << ident << "\n";

            if (it != keywords.end()) {
                return Token{it->second, ident, line, startColumn};
            }

            // Not a keyword: it's just a name (could be a variable)
            return Token{TokenType::IDENTIFIER, ident, line, startColumn};

        }

        Token lexNumber() {
            size_t start = position;
            int startCol = column;
            while (position < source.size() && std::isdigit(source[position])) {
                advance();
            }
            return Token{TokenType::INT_LITERAL, source.substr(start, position - start), line, startCol};
        }

        Token lexAssignOrEqual() {
            int startCol = column;
            advance(); // consume '='
            if (position < source.size() && source[position] == '=') {
                advance();
                return Token{TokenType::EQUAL, "==", line, startCol};
            } else {
                return Token{TokenType::ASSIGN, "=", line, startCol};
            }
        }
};