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
#include "ErrorReporter.h"

class ParseNode;

/**
 * table driven SLR parser
 */
class Parser {
public:
    // SLR tables
    Parser(const ParseTable &table, const std::vector<Production> &productions);

    // parse token stream
    bool parse(TokenStream &tokens);

    double lastParseMs() const { return lastLoopMs_; }

private:
    const ParseTable &table_;
    const std::vector<Production> &productions_;

    // pushdown automaton
    std::vector<int> stateStack_;
    std::vector<ParseNode *> nodeStack_;

    // syntax errors collected during the parse
    ErrorReporter errors_;

    double lastLoopMs_ = 0.0;

    // map lexer token to grammar terminal
    std::string terminalOf(const Token &token) const;

    void printProduction(const Production &production) const;

    // panic-mode recovery
    bool recover(TokenStream &tokens);

    // delete and clear any remaining parse tree nodes
    void freeNodes();
};



#endif //LR_PARSER_PARSER_H
