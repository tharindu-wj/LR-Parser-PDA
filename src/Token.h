//
// Created by thari on 7/06/2026.
//

#ifndef LR_PARSER_TOKEN_H
#define LR_PARSER_TOKEN_H

#include <string>

// token categories used in assessment 1 lexer
// additionally added end ($)
enum class TokenType {
    Keyword, Identifier, Number, Operator, Delimiter, Error, End
};

struct Token {
    TokenType   type;
    std::string lexeme;
    int lineNumber = 0;
    int columnNumber  = 0;
};

inline const char* tokenTypeName(TokenType t) {
    switch (t) {
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
