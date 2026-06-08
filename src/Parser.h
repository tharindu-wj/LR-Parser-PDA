//
// Created by thari on 8/06/2026.
//

#ifndef LR_PARSER_PARSER_H
#define LR_PARSER_PARSER_H

#include <string>
#include <vector>

#include "Token.h"
#include "Production.h"
#include "ParseTable.h"
#include "TokenStream.h"

/**
 * table driven SLR parser
 */
class Parser {
public:
    // SLR tables
    Parser(const ParseTable &table, const std::vector<Production> &productions);

    // parse token stream
    bool parse(TokenStream &tokens);

private:
    const ParseTable &table_;
    const std::vector<Production> &productions_;

    // map lexer token to grammar terminal
    std::string terminalOf(const Token &token) const;

    void printProduction(const Production &production) const;
};



#endif //LR_PARSER_PARSER_H
