//
// Created by thari on 7/06/2026.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdio>

#include "token.h"
#include "grammar.h"

// Map a type name from the lexer output back to a TokenType.
static TokenType tokenTypeFromName(const std::string& name) {
    if (name == "KEYWORD") return TokenType::Keyword;
    if (name == "IDENTIFIER") return TokenType::Identifier;
    if (name == "NUMBER") return TokenType::Number;
    if (name == "OPERATOR") return TokenType::Operator;
    if (name == "DELIMITER") return TokenType::Delimiter;
    return TokenType::Error;
}

static std::vector<Token> readTokens(std::istream& inputStream) {
    std::vector<Token> tokens;
    std::string currentLine;

    while (std::getline(inputStream, currentLine)) {
        const std::size_t colonPosition = currentLine.find(": ");
        if (colonPosition == std::string::npos) {
            continue;
        }

        // read the position: "Line <>, Column <>"
        int lineNumber = 0;
        int columnNumber = 0;
        std::sscanf(currentLine.c_str(), "Line %d, Column %d", &lineNumber, &columnNumber);

        // extraxt type name
        const std::string afterColon = currentLine.substr(colonPosition + 2);
        std::istringstream afterColonStream(afterColon);
        std::string typeName;
        afterColonStream >> typeName;

        // extract lexeme
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
        tokens.push_back(token);
    }

    // add the end of input marker
    Token endToken;
    endToken.type = TokenType::End;
    endToken.lexeme = "$";
    endToken.lineNumber = 0;
    endToken.columnNumber = 0;
    tokens.push_back(endToken);

    return tokens;
}

int main(int argc, char* argv[]) {
    // validate input file
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input-file>\n";
        return 1;
    }

    // load grammar from a file
    Grammar grammar;
    if (!loadGrammar(argv[1], grammar)) {
        std::cerr << "Error: could not load grammar '" << argv[1] << "'.\n";
        return 1;
    }
    printGrammar(grammar);

    if (argc >= 3) {
        std::ifstream tokenFile(argv[2]);
        if (!tokenFile) {
            std::cerr << "Error: could not open token file '" << argv[2] << "'.\n";
            return 1;
        }

        std::vector<Token> tokens = readTokens(tokenFile);
        std::cout << "\nRead " << tokens.size() << " tokens:\n";
        for (std::size_t tokenIndex = 0; tokenIndex < tokens.size(); ++tokenIndex) {
            const Token& token = tokens[tokenIndex];
            std::cout << "  " << tokenTypeName(token.type) << " '" << token.lexeme << "'"
                << "  (" << token.lineNumber << ":" << token.columnNumber << ")\n";
        }
    }

    return 0;
}