//
// Created by thari on 8/06/2026.
//

#ifndef LR_PARSER_GRAMMAR_H
#define LR_PARSER_GRAMMAR_H

#include <string>
#include <vector>
#include <set>

#include "Production.h"

/**
* Load the grammar rules from a file and keep track of terminals symbols and non-terminals symbols
 */
class Grammar {
public:
    // load a grammar file.
    bool load(const std::string &filePath);

    //print
    void print() const;

   // get start syymbol
    const std::string &getStartSymbol() const;

    // gett all productons
    const std::vector<Production> &getProductions() const;

    // get terminal symbols
    const std::set<std::string> &getTerminals() const;

    // get non-terminal symbols
    const std::set<std::string> &getNonTerminals() const;

    bool isNonTerminal(const std::string &symbolName) const;

private:
    std::vector<Production> productions_;
    std::set<std::string> nonTerminalSymbols_;
    std::set<std::string> terminalSymbols_;
    std::string startSymbol_;
};



#endif //LR_PARSER_GRAMMAR_H
