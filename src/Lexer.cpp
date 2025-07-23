#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include "../include/Lexer.hpp"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " filename\n";
        return 1;
    }

    std::string filename = argv[1];

    
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Could not open file: " << filename << "\n";
        return 1;
    };
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

    std::string contents(size, '\0');
    file.read(&contents[0], size);

    

    Lexer lexer(contents);
    Token token;
    do {
        token = lexer.getNextToken();
        std::cout << "Token: ";
        switch (token.type) {
            case TokenType::IDENTIFIER: std::cout << "IDENTIFIER"; break;
            case TokenType::KEYWORD: std::cout << "KEYWORD"; break;
            case TokenType::INT_LITERAL: std::cout << "INT_LITERAL"; break;
            case TokenType::FLOAT_LITERAL: std::cout << "FLOAT_LITERAL"; break;
            case TokenType::STRING_LITERAL: std::cout << "STRING_LITERAL"; break;
            case TokenType::BOOLEAN_LITERAL: std::cout << "BOOLEAN_LITERAL"; break;
            case TokenType::NULL_LITERAL: std::cout << "NULL_LITERAL"; break;
            case TokenType::ADD: std::cout << "PLUS"; break;
            case TokenType::SUBTRACT: std::cout << "MINUS"; break;
            case TokenType::MULTIPLY: std::cout << "MULTIPLY"; break;
            case TokenType::DIVIDE: std::cout << "DIVIDE"; break;
            case TokenType::ASSIGN: std::cout << "ASSIGN"; break;
            case TokenType::EQUAL: std::cout << "EQUAL"; break;
            case TokenType::NOT_EQUAL: std::cout << "NOT_EQUAL"; break;
            case TokenType::LESS_THAN: std::cout << "LESS_THAN"; break;
            case TokenType::LESS_THAN_OR_EQUAL: std::cout << "LESS_EQUAL"; break;
            case TokenType::GREATER_THAN: std::cout << "GREATER_THAN"; break;
            case TokenType::GREATER_THAN_OR_EQUAL: std::cout << "GREATER_EQUAL"; break;
            case TokenType::AND: std::cout << "AND"; break;
            case TokenType::OR: std::cout << "OR"; break;
            case TokenType::NOT: std::cout << "NOT"; break;
            case TokenType::LEFT_PARENTHESIS: std::cout << "LEFT_PAREN"; break;
            case TokenType::RIGHT_PARENTHISIS: std::cout << "RIGHT_PAREN"; break;
            case TokenType::LEFT_BRACKET: std::cout << "LEFT_BRACKET"; break;
            case TokenType::RIGHT_BRACKET: std::cout << "RIGHT_BRACKET"; break;
            case TokenType::LEFT_BRACE: std::cout << "LEFT_BRACE"; break;
            case TokenType::RIGHT_BRACE: std::cout << "RIGHT_BRACE"; break;
            case TokenType::COMMA: std::cout << "COMMA"; break;
            case TokenType::DOT: std::cout << "DOT"; break;
            case TokenType::SEMI_COLON: std::cout << "SEMI_COLON"; break;
            case TokenType::NEW_LINE: std::cout << "NEW_LINE"; break;
            case TokenType::COMMENT: std::cout << "COMMENT"; break;
            case TokenType::END_OF_FILE: std::cout << "END_OF_FILE"; break;
            case TokenType::UNKNOWN: std::cout << "UNKNOWN"; break;
            case TokenType::FUNCTION: std::cout << "FUNCTION"; break;
            case TokenType::IF: std::cout << "IF"; break;
            case TokenType::RETURN: std::cout << "RETURN"; break;
        };
        
        std::cout << ", Value: '" << token.value << "', Line: " << token.line << ", Col: " << token.column << "\n";
    } while (token.type != TokenType::END_OF_FILE);

    return 0;
}
