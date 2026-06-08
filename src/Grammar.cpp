//
// Created by thari on 7/06/2026.
//
#include <fstream>
#include <sstream>
#include <iostream>

#include "grammar.h"


// remove spaces and tabs
static std::string trimWhitespace(const std::string& text) {
    const std::size_t firstNonSpace = text.find_first_not_of(" \t\r\n");
    if (firstNonSpace == std::string::npos) {
        return "";
    }
    const std::size_t lastNonSpace = text.find_last_not_of(" \t\r\n");
    return text.substr(firstNonSpace, lastNonSpace - firstNonSpace + 1);
}

// turn one alternative into a Production.
static Production parseOneAlternative(const std::string& leftHandSide, const std::string& alternativeText) {
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

bool loadGrammar(const std::string& filePath, Grammar& grammar) {
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

        // split the line into a left hand side and a right-hand side at "->"
        const std::size_t arrowPosition = currentLine.find("->");
        if (arrowPosition == std::string::npos) {
            continue;
        }

        const std::string leftHandSide = trimWhitespace(currentLine.substr(0, arrowPosition));
        const std::string rightHandSidePart = trimWhitespace(currentLine.substr(arrowPosition + 2));

        grammar.nonTerminalSymbols.insert(leftHandSide);
        if (isFirstLeftHandSide) {
            grammar.startSymbol = leftHandSide;
            isFirstLeftHandSide = false;
        }

        // split the right hand side into alternatives at each '|'
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
            grammar.productions.push_back(production);
        }
    }

    // a symbol is a terminal if it was quoted
    // or if it never appears as a left hand side anywhere in the grammar
    for (std::size_t productionIndex = 0; productionIndex < grammar.productions.size(); ++productionIndex) {
        const Production& production = grammar.productions[productionIndex];
        for (std::size_t symbolIndex = 0; symbolIndex < production.rightHandSide.size(); ++symbolIndex) {
            const GrammarSymbol& grammarSymbol = production.rightHandSide[symbolIndex];
            bool appearsAsLeftHandSide = (grammar.nonTerminalSymbols.find(grammarSymbol.symbolName) != grammar.nonTerminalSymbols.end());
            if (grammarSymbol.isQuotedTerminal || !appearsAsLeftHandSide) {
                grammar.terminalSymbols.insert(grammarSymbol.symbolName);
            }
        }
    }

    return true;
}

void printGrammar(const Grammar& grammar) {
    std::cout << "Start symbol: " << grammar.startSymbol << "\n\n";

    std::cout << "Productions (" << grammar.productions.size() << "):\n";
    int productionNumber = 0;
    for (std::size_t productionIndex = 0; productionIndex < grammar.productions.size(); ++productionIndex) {
        const Production& production = grammar.productions[productionIndex];
        std::cout << "  " << productionNumber << ": " << production.leftHandSide << " ->";
        if (production.rightHandSide.empty()) {
            std::cout << " (epsilon)";
        } else {
            for (std::size_t symbolIndex = 0; symbolIndex < production.rightHandSide.size(); ++symbolIndex) {
                const GrammarSymbol& grammarSymbol = production.rightHandSide[symbolIndex];
                if (grammarSymbol.isQuotedTerminal) {
                    std::cout << " \"" << grammarSymbol.symbolName << "\"";
                } else {
                    std::cout << " " << grammarSymbol.symbolName;
                }
            }
        }
        std::cout << "\n";
        productionNumber++;
    }

    std::cout << "\nNon-terminals (" << grammar.nonTerminalSymbols.size() << "): ";
    for (const std::string& nonTerminalName : grammar.nonTerminalSymbols) {
        std::cout << nonTerminalName << " ";
    }

    std::cout << "\nTerminals (" << grammar.terminalSymbols.size() << "): ";
    for (const std::string& terminalName : grammar.terminalSymbols) {
        std::cout << terminalName << " ";
    }
    std::cout << "\n";
}