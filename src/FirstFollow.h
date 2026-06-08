//
// Created by thari on 8/06/2026.
//

#ifndef LR_PARSER_FIRSTFOLLOW_H
#define LR_PARSER_FIRSTFOLLOW_H

#include <string>
#include <map>
#include <set>

#include "Grammar.h"

class FirstFollow {
public:
    // works out the FIRST and FOLLOW sets from the grammar
    void compute(const Grammar &grammar);

    // print the FIRST and FOLLOW sets
    void print(const Grammar &grammar) const;

    // get FIRST set of a non-terminal
    const std::set<std::string> &firstOf(const std::string &nonTerminalName) const;

    // get FOLLOW set of a non termina
    const std::set<std::string> &followOf(const std::string &nonTerminalName) const;

private:
    std::map<std::string, std::set<std::string>> first_;
    std::map<std::string, std::set<std::string>> follow_;

    void computeFirstSets(const Grammar &grammar);
    void computeFollowSets(const Grammar &grammar);
};



#endif //LR_PARSER_FIRSTFOLLOW_H
