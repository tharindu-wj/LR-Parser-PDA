//
// Created by thari on 8/06/2026.
//

#include "Parser.h"

#include <iostream>

#include "ParseNode.h"
#include "TerminalNode.h"
#include "RuleNode.h"

Parser::Parser(const ParseTable &table, const std::vector<Production> &productions)
    : table_(table), productions_(productions) {
}

std::string Parser::terminalOf(const Token &token) const {
    switch (token.type) {
        case TokenType::Identifier: return "Identifier";
        case TokenType::Number: return "Number";
        case TokenType::End: return "$";
        case TokenType::Keyword:
        case TokenType::Operator:
        case TokenType::Delimiter:
            return token.lexeme;
        case TokenType::Error:
            return token.lexeme;
    }
    return token.lexeme;
}

void Parser::printProduction(const Production &production) const {
    std::cout << production.leftHandSide << " ->";
    if (production.rightHandSide.empty()) {
        std::cout << " (epsilon)";
        return;
    }
    for (std::size_t i = 0; i < production.rightHandSide.size(); ++i) {
        const GrammarSymbol &symbol = production.rightHandSide[i];
        if (symbol.isQuotedTerminal) {
            std::cout << " \"" << symbol.symbolName << "\"";
        } else {
            std::cout << " " << symbol.symbolName;
        }
    }
}

bool Parser::parse(TokenStream &tokens) {
    // PDA stack holds state numbers
    std::vector<int> stateStack;
    stateStack.push_back(0);

    // parse tree is built bottom-up
    std::vector<ParseNode *> nodeStack;

    std::cout << "\nParsing:\n";

    while (true) {
        int currentState = stateStack.back();
        const Token &lookahead = tokens.peek();
        std::string terminal = terminalOf(lookahead);

        ActionEntry entry = table_.action(currentState, terminal);

        if (entry.kind == ActionEntry::Kind::Shift) {
            std::cout << "  shift '" << terminal << "' -> state " << entry.value << "\n";
            stateStack.push_back(entry.value);
            nodeStack.push_back(new TerminalNode(lookahead));
            tokens.advance();

        } else if (entry.kind == ActionEntry::Kind::Reduce) {
            const Production &production = productions_[entry.value];
            std::size_t rhsLength = production.rightHandSide.size();

            std::vector<ParseNode *> children;
            for (std::size_t i = nodeStack.size() - rhsLength; i < nodeStack.size(); ++i) {
                children.push_back(nodeStack[i]);
            }

            // pop the right-hand side off both stacks
            for (std::size_t i = 0; i < rhsLength; ++i) {
                stateStack.pop_back();
                nodeStack.pop_back();
            }

            // move to the GOTO state for the rule left-hand side
            int exposedState = stateStack.back();
            int gotoTarget = table_.gotoState(exposedState, production.leftHandSide);
            if (gotoTarget == -1) {
                std::cerr << "Internal error: no GOTO for " << production.leftHandSide
                    << " in state " << exposedState << "\n";
                for (std::size_t i = 0; i < children.size(); ++i) {
                    delete children[i];
                }
                for (std::size_t i = 0; i < nodeStack.size(); ++i) {
                    delete nodeStack[i];
                }
                std::cout << "\nResult: input REJECTED\n";
                return false;
            }
            stateStack.push_back(gotoTarget);
            nodeStack.push_back(new RuleNode(production.leftHandSide, children));

            std::cout << "  reduce by rule " << entry.value << ": ";
            printProduction(production);
            std::cout << "\n";

        } else if (entry.kind == ActionEntry::Kind::Accept) {
            std::cout << "  accept\n";
            std::cout << "\nResult: input ACCEPTED\n";

            std::cout << "\nParse tree:\n";
            if (!nodeStack.empty()) {
                ParseNode *root = nodeStack.back();
                nodeStack.pop_back();
                root->print(0);
                delete root;
            }
            for (std::size_t i = 0; i < nodeStack.size(); ++i) {
                delete nodeStack[i];
            }

            return true;

        } else {
            std::cerr << "Syntax error at line " << lookahead.lineNumber
                << ", column " << lookahead.columnNumber
                << ": unexpected " << tokenTypeName(lookahead.type)
                << " '" << lookahead.lexeme << "'\n";

            for (std::size_t i = 0; i < nodeStack.size(); ++i) {
                delete nodeStack[i];
            }

            std::cout << "\nResult: input REJECTED\n";
            return false;
        }
    }
}