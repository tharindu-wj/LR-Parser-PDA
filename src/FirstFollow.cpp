//
// Created by thari on 8/06/2026.
//

#include "FirstFollow.h"

#include <iostream>

static const std::string EPSILON = "<eps>";

// add one element to a set
static bool addOne(std::set<std::string> &target, const std::string &element) {
    return target.insert(element).second;
}

// add all element of source into target
static bool addAll(std::set<std::string> &target, const std::set<std::string> &source) {
    bool addedSomething = false;
    for (const std::string &element : source) {
        if (target.insert(element).second) {
            addedSomething = true;
        }
    }
    return addedSomething;
}

// build FIRST set for every non-terminal
void FirstFollow::computeFirstSets(const Grammar &grammar) {
    const std::vector<Production> &productions = grammar.getProductions();

    for (const std::string &nonTerminalName : grammar.getNonTerminals()) {
        first_[nonTerminalName] = std::set<std::string>();
    }

    bool changed = true;
    while (changed) {
        changed = false;

        for (std::size_t productionIndex = 0; productionIndex < productions.size(); ++productionIndex) {
            const Production &production = productions[productionIndex];
            const std::string leftSide = production.leftHandSide;

            // an empty rule: A -> epsilon
            // A can start with nothing
            if (production.rightHandSide.empty()) {
                if (addOne(first_[leftSide], EPSILON)) {
                    changed = true;
                }
                continue;
            }

            // iterate the right hand side symbol by symbol
            bool allSymbolsCanBeEmpty = true;
            for (std::size_t symbolIndex = 0; symbolIndex < production.rightHandSide.size(); ++symbolIndex) {
                const std::string symbolName = production.rightHandSide[symbolIndex].symbolName;

                if (!grammar.isNonTerminal(symbolName)) {
                    // a terminal: FIRST is just the terminal itself
                    if (addOne(first_[leftSide], symbolName)) {
                        changed = true;
                    }
                    allSymbolsCanBeEmpty = false;
                    break;
                }

                // a non-terminal: add its FIRST set without epsilon
                std::set<std::string> firstWithoutEpsilon = first_[symbolName];
                firstWithoutEpsilon.erase(EPSILON);
                if (addAll(first_[leftSide], firstWithoutEpsilon)) {
                    changed = true;
                }

                // if this non-terminal cannot be empty
                // break iteration rule
                bool symbolCanBeEmpty = (first_[symbolName].find(EPSILON) != first_[symbolName].end());
                if (!symbolCanBeEmpty) {
                    allSymbolsCanBeEmpty = false;
                    break;
                }
            }

            if (allSymbolsCanBeEmpty) {
                if (addOne(first_[leftSide], EPSILON)) {
                    changed = true;
                }
            }
        }
    }
}

// build the FOLLOW set for every non-terminal
void FirstFollow::computeFollowSets(const Grammar &grammar) {
    const std::vector<Production> &productions = grammar.getProductions();

    for (const std::string &nonTerminalName : grammar.getNonTerminals()) {
        follow_[nonTerminalName] = std::set<std::string>();
    }

    follow_[grammar.getStartSymbol()].insert("$");

    bool changed = true;
    while (changed) {
        changed = false;

        for (std::size_t productionIndex = 0; productionIndex < productions.size(); ++productionIndex) {
            const Production &production = productions[productionIndex];
            const std::string leftSide = production.leftHandSide;

            for (std::size_t symbolIndex = 0; symbolIndex < production.rightHandSide.size(); ++symbolIndex) {
                const std::string symbolName = production.rightHandSide[symbolIndex].symbolName;

                if (!grammar.isNonTerminal(symbolName)) {
                    continue;
                }

                // look at everything that comes after this symbol in the rule
                bool restCanBeEmpty = true;
                for (std::size_t afterIndex = symbolIndex + 1; afterIndex < production.rightHandSide.size(); ++afterIndex) {
                    const std::string nextName = production.rightHandSide[afterIndex].symbolName;

                    if (!grammar.isNonTerminal(nextName)) {
                        // a terminal: add it and break iteration
                        if (addOne(follow_[symbolName], nextName)) {
                            changed = true;
                        }
                        restCanBeEmpty = false;
                        break;
                    }

                    // a non-terminal: add its FIRST set without epsilon
                    std::set<std::string> firstWithoutEpsilon = first_[nextName];
                    firstWithoutEpsilon.erase(EPSILON);
                    if (addAll(follow_[symbolName], firstWithoutEpsilon)) {
                        changed = true;
                    }

                    bool nextCanBeEmpty = (first_[nextName].find(EPSILON) != first_[nextName].end());
                    if (!nextCanBeEmpty) {
                        restCanBeEmpty = false;
                        break;
                    }
                }

                if (restCanBeEmpty) {
                    std::set<std::string> followOfLeftSide = follow_[leftSide];
                    if (addAll(follow_[symbolName], followOfLeftSide)) {
                        changed = true;
                    }
                }
            }
        }
    }
}

void FirstFollow::compute(const Grammar &grammar) {
    computeFirstSets(grammar);
    computeFollowSets(grammar);
}

void FirstFollow::print(const Grammar &grammar) const {
    std::cout << "\nFIRST sets:\n";
    for (const std::string &nonTerminalName : grammar.getNonTerminals()) {
        std::cout << "  FIRST(" << nonTerminalName << ") = { ";
        const std::set<std::string> &firstSet = first_.at(nonTerminalName);
        for (const std::string &symbolName : firstSet) {
            std::cout << symbolName << " ";
        }
        std::cout << "}\n";
    }

    std::cout << "\nFOLLOW sets:\n";
    for (const std::string &nonTerminalName : grammar.getNonTerminals()) {
        std::cout << "  FOLLOW(" << nonTerminalName << ") = { ";
        const std::set<std::string> &followSet = follow_.at(nonTerminalName);
        for (const std::string &symbolName : followSet) {
            std::cout << symbolName << " ";
        }
        std::cout << "}\n";
    }
}

const std::set<std::string> &FirstFollow::firstOf(const std::string &nonTerminalName) const {
    return first_.at(nonTerminalName);
}

const std::set<std::string> &FirstFollow::followOf(const std::string &nonTerminalName) const {
    return follow_.at(nonTerminalName);
}