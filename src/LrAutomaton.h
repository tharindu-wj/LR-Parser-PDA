//
// Created by thari on 8/06/2026.
//

#ifndef LR_PARSER_LRAUTOMATON_H
#define LR_PARSER_LRAUTOMATON_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>

#include "Grammar.h"

class LrAutomaton {
public:
    // build automaton from the grammar
    void build(const Grammar &grammar);

    void print() const;

    std::size_t stateCount() const;

private:
    // LR(0) item
    struct Item {
        int productionIndex;
        std::size_t dotPosition;

        bool operator<(const Item &other) const {
            if (productionIndex != other.productionIndex) {
                return productionIndex < other.productionIndex;
            }
            return dotPosition < other.dotPosition;
        }
        bool operator==(const Item &other) const {
            return productionIndex == other.productionIndex && dotPosition == other.dotPosition;
        }
    };

    std::vector<Production> augmentedProductions_;
    std::string augmentedStartSymbol_;
    std::vector<std::set<Item>> states_;
    std::map<std::pair<int, std::string>, int> transitions_;

    std::set<Item> closure(const std::set<Item> &items, const Grammar &grammar) const;
    std::set<Item> gotoState(const std::set<Item> &items, const std::string &symbol, const Grammar &grammar) const;
    int findState(const std::set<Item> &items) const;
    void printItem(const Item &item) const;
};

#endif //LR_PARSER_LRAUTOMATON_H
