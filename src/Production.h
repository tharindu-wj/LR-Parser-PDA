//
// Created by thari on 8/06/2026.
//

#ifndef LR_PARSER_PRODUCTION_H
#define LR_PARSER_PRODUCTION_H

#include <string>
#include <vector>

/**
 * symbol on the right-hand side of a grammar rule
 */
struct GrammarSymbol {
    std::string symbolName;
    bool isQuotedTerminal = false;
};

/**
 * grammar rule: leftHandSide -> rightHandSide
 */
struct Production {
    std::string leftHandSide;
    std::vector<GrammarSymbol> rightHandSide;
};



#endif //LR_PARSER_PRODUCTION_H
