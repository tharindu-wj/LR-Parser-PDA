//
// Created by thari on 8/06/2026.
//

#include "LrAutomaton.h"

#include <iostream>

// CLOSURE(I)
std::set<LrAutomaton::Item> LrAutomaton::closure(const std::set<Item> &items, const Grammar &grammar) const {
    std::set<Item> result = items;

    bool changed = true;
    while (changed) {
        changed = false;

        std::set<Item> snapshot = result;
        for (const Item &item : snapshot) {
            const Production &production = augmentedProductions_[item.productionIndex];

            // nothing to expand if the dot is at the end
            if (item.dotPosition >= production.rightHandSide.size()) {
                continue;
            }

            const std::string nextSymbol = production.rightHandSide[item.dotPosition].symbolName;

            // only non-terminals get expanded
            if (!grammar.isNonTerminal(nextSymbol)) {
                continue;
            }

            // add dot at the start for every rule of nextSymbol
            for (std::size_t p = 0; p < augmentedProductions_.size(); ++p) {
                if (augmentedProductions_[p].leftHandSide == nextSymbol) {
                    Item newItem;
                    newItem.productionIndex = (int) p;
                    newItem.dotPosition = 0;
                    if (result.insert(newItem).second) {
                        changed = true;
                    }
                }
            }
        }
    }

    return result;
}

// GOTO(I, X)
std::set<LrAutomaton::Item> LrAutomaton::gotoState(const std::set<Item> &items, const std::string &symbol, const Grammar &grammar) const {
    std::set<Item> moved;
    for (const Item &item : items) {
        const Production &production = augmentedProductions_[item.productionIndex];
        if (item.dotPosition < production.rightHandSide.size() &&
            production.rightHandSide[item.dotPosition].symbolName == symbol) {
            Item shifted;
            shifted.productionIndex = item.productionIndex;
            shifted.dotPosition = item.dotPosition + 1;
            moved.insert(shifted);
            }
    }
    return closure(moved, grammar);
}

// get index of existing state equal to items
int LrAutomaton::findState(const std::set<Item> &items) const {
    for (std::size_t i = 0; i < states_.size(); ++i) {
        if (states_[i] == items) {
            return (int) i;
        }
    }
    return -1;
}

void LrAutomaton::build(const Grammar &grammar) {
    // rule 0: Start' -> Start
    augmentedStartSymbol_ = grammar.getStartSymbol() + "'";
    Production augmentedRule;
    augmentedRule.leftHandSide = augmentedStartSymbol_;
    GrammarSymbol startSymbol;
    startSymbol.symbolName = grammar.getStartSymbol();
    startSymbol.isQuotedTerminal = false;
    augmentedRule.rightHandSide.push_back(startSymbol);
    augmentedProductions_.push_back(augmentedRule);

    const std::vector<Production> &grammarProductions = grammar.getProductions();
    for (std::size_t i = 0; i < grammarProductions.size(); ++i) {
        augmentedProductions_.push_back(grammarProductions[i]);
    }

    // state 0: closure of { Start' -> . Start }
    Item startItem;
    startItem.productionIndex = 0;
    startItem.dotPosition = 0;
    std::set<Item> startSet;
    startSet.insert(startItem);
    states_.push_back(closure(startSet, grammar));

    // every symbol that after a dot
    std::vector<std::string> allSymbols;
    for (const std::string &terminalName : grammar.getTerminals()) {
        allSymbols.push_back(terminalName);
    }
    for (const std::string &nonTerminalName : grammar.getNonTerminals()) {
        allSymbols.push_back(nonTerminalName);
    }

    // for each state and each symbol compute GOTO
    for (std::size_t stateIndex = 0; stateIndex < states_.size(); ++stateIndex) {
        for (std::size_t symbolIndex = 0; symbolIndex < allSymbols.size(); ++symbolIndex) {
            const std::string symbol = allSymbols[symbolIndex];
            std::set<Item> target = gotoState(states_[stateIndex], symbol, grammar);
            if (target.empty()) {
                continue;
            }
            int existing = findState(target);
            if (existing == -1) {
                states_.push_back(target);
                existing = (int) states_.size() - 1;
            }
            transitions_[std::make_pair((int) stateIndex, symbol)] = existing;
        }
    }
}

void LrAutomaton::printItem(const Item &item) const {
    const Production &production = augmentedProductions_[item.productionIndex];
    std::cout << production.leftHandSide << " ->";
    for (std::size_t k = 0; k <= production.rightHandSide.size(); ++k) {
        if (k == item.dotPosition) {
            std::cout << " .";
        }
        if (k < production.rightHandSide.size()) {
            const GrammarSymbol &symbol = production.rightHandSide[k];
            if (symbol.isQuotedTerminal) {
                std::cout << " \"" << symbol.symbolName << "\"";
            } else {
                std::cout << " " << symbol.symbolName;
            }
        }
    }
    std::cout << "\n";
}

void LrAutomaton::print() const {
    std::cout << "\nAugmented grammar (" << augmentedProductions_.size() << " rules):\n";
    for (std::size_t p = 0; p < augmentedProductions_.size(); ++p) {
        const Production &production = augmentedProductions_[p];
        std::cout << "  " << p << ": " << production.leftHandSide << " ->";
        for (std::size_t k = 0; k < production.rightHandSide.size(); ++k) {
            const GrammarSymbol &symbol = production.rightHandSide[k];
            if (symbol.isQuotedTerminal) {
                std::cout << " \"" << symbol.symbolName << "\"";
            } else {
                std::cout << " " << symbol.symbolName;
            }
        }
        std::cout << "\n";
    }

    std::cout << "\nLR(0) automaton (" << states_.size() << " states):\n";
    for (std::size_t i = 0; i < states_.size(); ++i) {
        std::cout << "State " << i << ":\n";
        for (const Item &item : states_[i]) {
            std::cout << "  ";
            printItem(item);
        }
    }

    std::cout << "\nTransitions:\n";
    for (const std::pair<const std::pair<int, std::string>, int> &entry : transitions_) {
        std::cout << "  State " << entry.first.first << " --" << entry.first.second
            << "--> State " << entry.second << "\n";
    }
}

std::size_t LrAutomaton::stateCount() const {
    return states_.size();
}

const std::vector<Production> &LrAutomaton::getAugmentedProductions() const {
    return augmentedProductions_;
}

const std::map<std::pair<int, std::string>, int> &LrAutomaton::getTransitions() const {
    return transitions_;
}

std::vector<int> LrAutomaton::reducibleProductions(int state) const {
    std::vector<int> result;
    for (const Item &item : states_[state]) {
        if (item.dotPosition == augmentedProductions_[item.productionIndex].rightHandSide.size()) {
            result.push_back(item.productionIndex);
        }
    }
    return result;
}