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

// Map a type name from the lexer output back to a TokenType.
static TokenType typeFromName(const std::string& name) {
    if (name == "KEYWORD") return TokenType::Keyword;
    if (name == "IDENTIFIER") return TokenType::Identifier;
    if (name == "NUMBER") return TokenType::Number;
    if (name == "OPERATOR") return TokenType::Operator;
    if (name == "DELIMITER") return TokenType::Delimiter;
    return TokenType::Error;
}

static std::vector<Token> readTokens(std::istream& in) {
    std::vector<Token> tokens;
    std::string raw;

    // read line by line
    while (std::getline(in, raw)) {
        std::size_t colon = raw.find(": ");
        if (colon == std::string::npos) {
            continue;
        }

        // position: "Line <>, Column <>"
        int line = 0, col = 0;
        std::sscanf(raw.c_str(), "Line %d, Column %d", &line, &col);

        // after the colon: TYPE then "lexeme"
        const std::string rest = raw.substr(colon + 2);
        std::istringstream ss(rest);
        std::string typeName;
        ss >> typeName;

        const std::size_t q1 = rest.find('"');
        const std::size_t q2 = rest.rfind('"');
        std::string lexeme;
        if (q1 != std::string::npos && q2 != std::string::npos && q2 > q1) {
            lexeme = rest.substr(q1 + 1, q2 - q1 - 1);
        }

        tokens.push_back({ typeFromName(typeName), lexeme, line, col });
    }

    tokens.push_back({ TokenType::End, "$", 0, 0 });   // end-of-input marker
    return tokens;
}

int main(int argc, char* argv[]) {
    // validate input file
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input-file>\n";
        return 1;
    }

    // open input file.
    std::ifstream input(argv[1]);
    if (!input) {
        std::cout << "Error: could not open file '" << argv[1] << "'.\n";
        return 1;
    }

    const std::vector<Token> tokens = readTokens(input);

    std::cout << "Read " << tokens.size() << " tokens:\n";
    for (const Token& t : tokens) {
        std::cout << "  " << tokenTypeName(t.type)
                  << " '" << t.lexeme << "'"
                  << "  (" << t.line << ":" << t.col << ")\n";
    }

    return 0;
}