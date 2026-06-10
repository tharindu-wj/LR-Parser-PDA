//
// Created by thari on 8/06/2026.
//

#ifndef LR_PARSER_PARSETABLE_H
#define LR_PARSER_PARSETABLE_H

#include <string>
#include <vector>
#include <map>
#include <utility>

#include "Grammar.h"
#include "FirstFollow.h"
#include "LrAutomaton.h"

struct ActionEntry {
    enum class Kind { Error, Shift, Reduce, Accept };
    Kind kind = Kind::Error;
    int value = 0;   // Shift: target state ; Reduce: production index
};

class ParseTable {
public:
    // build the SLR(1) ACTION table and GOTO table
    void build(const Grammar &grammar, const FirstFollow &firstFollow, const LrAutomaton &automaton);

    void print() const;

    // get action for a state and terminal
    ActionEntry action(int state, const std::string &terminal) const;

    // get GOTO state for state and non-terminal
    int gotoState(int state, const std::string &nonTerminal) const;

    bool hasConflicts() const;

private:
    std::map<std::pair<int, std::string>, ActionEntry> action_;
    std::map<std::pair<int, std::string>, int> goto_;
    std::vector<std::string> conflicts_;

    void setAction(int state, const std::string &terminal, const ActionEntry &entry);
};



#endif //LR_PARSER_PARSETABLE_H
