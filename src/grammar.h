//
// Created by thari on 7/06/2026.
//

#ifndef LR_PARSER_GRAMMAR_H
#define LR_PARSER_GRAMMAR_H

#include <string>
#include <vector>
#include <set>


struct GrammarSymbol {
    std::string symbolName;
    bool isQuotedTerminal = false;
};

// one grammar rule: leftHandSide -> rightHandSide
// empty rightHandSide means the empty rule
struct Production {
    std::string leftHandSide;
    std::vector<GrammarSymbol> rightHandSide;
};

// grammar loaded from a file
struct Grammar {
    std::vector<Production> productions;
    std::set<std::string> nonTerminalSymbols;
    std::set<std::string> terminalSymbols;
    std::string startSymbol;
};

// load a grammar.txt file into the Grammar structure.
bool loadGrammar(const std::string& filePath, Grammar& grammar);

void printGrammar(const Grammar& grammar);

#endif //LR_PARSER_GRAMMAR_H
