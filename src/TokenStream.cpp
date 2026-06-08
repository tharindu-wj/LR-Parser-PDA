//
// Created by thari on 8/06/2026.
//

#include "TokenStream.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>

static TokenType tokenTypeFromName(const std::string &typeName) {
    if (typeName == "KEYWORD") return TokenType::Keyword;
    if (typeName == "IDENTIFIER") return TokenType::Identifier;
    if (typeName == "NUMBER") return TokenType::Number;
    if (typeName == "OPERATOR") return TokenType::Operator;
    if (typeName == "DELIMITER") return TokenType::Delimiter;
    return TokenType::Error;
}

bool TokenStream::loadFromFile(const std::string &filePath) {
    std::ifstream inputStream(filePath);
    if (!inputStream) {
        return false;
    }

    std::string currentLine;
    while (std::getline(inputStream, currentLine)) {
        const std::size_t colonPosition = currentLine.find(": ");
        if (colonPosition == std::string::npos) {
            continue;
        }

        // read the position: Line <>, Column <>
        int lineNumber = 0;
        int columnNumber = 0;
        std::sscanf(currentLine.c_str(), "Line %d, Column %d", &lineNumber, &columnNumber);

        // the part after the ; is type name and the "lexeme"
        const std::string afterColon = currentLine.substr(colonPosition + 2);
        std::istringstream afterColonStream(afterColon);
        std::string typeName;
        afterColonStream >> typeName;

        // lexeme: text between the first and last double quote
        const std::size_t firstQuotePosition = afterColon.find('"');
        const std::size_t lastQuotePosition = afterColon.rfind('"');
        std::string lexeme;
        if (firstQuotePosition != std::string::npos &&
            lastQuotePosition != std::string::npos &&
            lastQuotePosition > firstQuotePosition) {
            lexeme = afterColon.substr(firstQuotePosition + 1, lastQuotePosition - firstQuotePosition - 1);
            }

        Token token;
        token.type = tokenTypeFromName(typeName);
        token.lexeme = lexeme;
        token.lineNumber = lineNumber;
        token.columnNumber = columnNumber;
        tokens_.push_back(token);
    }

    // add the end-of-input marker
    Token endToken;
    endToken.type = TokenType::End;
    endToken.lexeme = "$";
    endToken.lineNumber = 0;
    endToken.columnNumber = 0;
    tokens_.push_back(endToken);

    return true;
}

void TokenStream::print() const {
    std::cout << "\nRead " << tokens_.size() << " tokens:\n";
    for (std::size_t tokenIndex = 0; tokenIndex < tokens_.size(); ++tokenIndex) {
        const Token &token = tokens_[tokenIndex];
        std::cout << "  " << tokenTypeName(token.type) << " '" << token.lexeme << "'"
            << "  (" << token.lineNumber << ":" << token.columnNumber << ")\n";
    }
}

const Token &TokenStream::peek() const {
    return tokens_[position_];
}

void TokenStream::advance() {
    if (position_ + 1 < tokens_.size()) {
        position_++;
    }
}

bool TokenStream::atEnd() const {
    return tokens_[position_].type == TokenType::End;
}

std::size_t TokenStream::size() const {
    return tokens_.size();
}