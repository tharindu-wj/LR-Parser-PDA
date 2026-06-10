//
// Created by thari on 8/06/2026.
//

#ifndef LR_PARSER_TOKEN_H
#define LR_PARSER_TOKEN_H

#include <string>

// tokens defined in the lexer
// End ($) is used to identify input finished
enum class TokenType {
    Keyword,
    Identifier,
    Number,
    Operator,
    Delimiter,
    Error,
    End
};

struct Token {
    TokenType type;
    std::string lexeme;
    int lineNumber = 0;
    int columnNumber = 0;
};

constexpr const char *tokenTypeName(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::Keyword: return "KEYWORD";
        case TokenType::Identifier: return "IDENTIFIER";
        case TokenType::Number: return "NUMBER";
        case TokenType::Operator: return "OPERATOR";
        case TokenType::Delimiter: return "DELIMITER";
        case TokenType::Error: return "ERROR";
        case TokenType::End: return "END";
    }
    return "?";
}

#endif //LR_PARSER_TOKEN_H
