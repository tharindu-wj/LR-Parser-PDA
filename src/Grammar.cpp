//
// Created by thari on 8/06/2026.
//

#include "Grammar.h"

#include <fstream>
#include <sstream>
#include <iostream>

// remove spaces and tabs
static std::string trimWhitespace(const std::string &text) {
    const std::size_t firstNonSpace = text.find_first_not_of(" \t\r\n");
    if (firstNonSpace == std::string::npos) {
        return "";
    }
    const std::size_t lastNonSpace = text.find_last_not_of(" \t\r\n");
    return text.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
}

// turn one alternative into a Production.
static Production parseOneAlternative(const std::string &leftHandSide, const std::string &alternativeText) {
    Production production;
    production.leftHandSide = leftHandSide;

    std::istringstream alternativeStream(alternativeText);
    std::string currentWord;
    while (alternativeStream >> currentWord) {
        // leave the right hand side empty
        if (currentWord == "%empty") {
            continue;
        }

        GrammarSymbol grammarSymbol;
        bool startsWithQuote = (currentWord.size() >= 2 && currentWord.front() == '"');
        bool endsWithQuote = (currentWord.size() >= 2 && currentWord.back() == '"');

        if (startsWithQuote && endsWithQuote) {
            // quoted terminal like "+" or "class"
            // remove the two quote characters
            grammarSymbol.symbolName = currentWord.substr(1, currentWord.size() - 2);
            grammarSymbol.isQuotedTerminal = true;
        } else {
            // bare symbol: Identifier, Number, or a non-terminal name
            grammarSymbol.symbolName = currentWord;
            grammarSymbol.isQuotedTerminal = false;
        }

        production.rightHandSide.push_back(grammarSymbol);
    }

    return production;
}

bool Grammar::load(const std::string &filePath) {
    std::ifstream grammarFile(filePath);
    if (!grammarFile) {
        return false;
    }

    std::string currentLine;
    bool isFirstLeftHandSide = true;

    while (std::getline(grammarFile, currentLine)) {
        // remove anything after a '#' comment marker
        const std::size_t commentStart = currentLine.find('#');
        if (commentStart != std::string::npos) {
            currentLine = currentLine.substr(0, commentStart);
        }
        currentLine = trimWhitespace(currentLine);
        if (currentLine.empty()) {
            continue;
        }

        // split the line into a left-hand side and a right-hand side at "->"
        const std::size_t arrowPosition = currentLine.find("->");
        if (arrowPosition == std::string::npos) {
            continue;
        }

        const std::string leftHandSide = trimWhitespace(currentLine.substr(0, arrowPosition));
        const std::string rightHandSidePart = trimWhitespace(currentLine.substr(arrowPosition + 2));

        nonTerminalSymbols_.insert(leftHandSide);
        if (isFirstLeftHandSide) {
            startSymbol_ = leftHandSide;
            isFirstLeftHandSide = false;
        }

        // split the right-hand side into alternatives at each '|'
        std::vector<std::string> alternatives;
        std::string currentAlternative;
        for (std::size_t characterIndex = 0; characterIndex < rightHandSidePart.size(); ++characterIndex) {
            char character = rightHandSidePart[characterIndex];
            if (character == '|') {
                alternatives.push_back(currentAlternative);
                currentAlternative = "";
            } else {
                currentAlternative += character;
            }
        }
        alternatives.push_back(currentAlternative);

        // make one production for each alternative
        for (std::size_t alternativeIndex = 0; alternativeIndex < alternatives.size(); ++alternativeIndex) {
            std::string oneAlternative = trimWhitespace(alternatives[alternativeIndex]);
            Production production = parseOneAlternative(leftHandSide, oneAlternative);
            productions_.push_back(production);
        }
    }

    for (std::size_t productionIndex = 0; productionIndex < productions_.size(); ++productionIndex) {
        const Production &production = productions_[productionIndex];
        for (std::size_t symbolIndex = 0; symbolIndex < production.rightHandSide.size(); ++symbolIndex) {
            const GrammarSymbol &grammarSymbol = production.rightHandSide[symbolIndex];
            bool appearsAsLeftHandSide = (nonTerminalSymbols_.find(grammarSymbol.symbolName) != nonTerminalSymbols_.end());
            if (grammarSymbol.isQuotedTerminal || !appearsAsLeftHandSide) {
                terminalSymbols_.insert(grammarSymbol.symbolName);
            }
        }
    }

    return true;
}

void Grammar::print() const {
    std::cout << "Start symbol: " << startSymbol_ << "\n\n";

    std::cout << "Productions (" << productions_.size() << "):\n";
    for (std::size_t productionIndex = 0; productionIndex < productions_.size(); ++productionIndex) {
        const Production &production = productions_[productionIndex];
        std::cout << "  " << productionIndex << ": " << production.leftHandSide << " ->";
        if (production.rightHandSide.empty()) {
            std::cout << " (epsilon)";
        } else {
            for (std::size_t symbolIndex = 0; symbolIndex < production.rightHandSide.size(); ++symbolIndex) {
                const GrammarSymbol &grammarSymbol = production.rightHandSide[symbolIndex];
                if (grammarSymbol.isQuotedTerminal) {
                    std::cout << " \"" << grammarSymbol.symbolName << "\"";
                } else {
                    std::cout << " " << grammarSymbol.symbolName;
                }
            }
        }
        std::cout << "\n";
    }

    std::cout << "\nNon-terminals (" << nonTerminalSymbols_.size() << "): ";
    for (const std::string &nonTerminalName : nonTerminalSymbols_) {
        std::cout << nonTerminalName << " ";
    }

    std::cout << "\nTerminals (" << terminalSymbols_.size() << "): ";
    for (const std::string &terminalName : terminalSymbols_) {
        std::cout << terminalName << " ";
    }
    std::cout << "\n";
}

const std::string &Grammar::getStartSymbol() const {
    return startSymbol_;
}

const std::vector<Production> &Grammar::getProductions() const {
    return productions_;
}

const std::set<std::string> &Grammar::getTerminals() const {
    return terminalSymbols_;
}

const std::set<std::string> &Grammar::getNonTerminals() const {
    return nonTerminalSymbols_;
}

bool Grammar::isNonTerminal(const std::string &symbolName) const {
    return nonTerminalSymbols_.find(symbolName) != nonTerminalSymbols_.end();
}