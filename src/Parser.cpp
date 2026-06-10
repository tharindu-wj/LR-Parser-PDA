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

void Parser::freeNodes() {
    for (std::size_t i = 0; i < nodeStack_.size(); ++i) {
        delete nodeStack_[i];
    }
    nodeStack_.clear();
}

bool Parser::recover(TokenStream &tokens) {
    static const char *syncNonTerminals[] = {
        "Statement", "MemberDeclaration", "ClassDeclaration"
    };

    // pop the stack until a state can GOTO on a sync non-terminal
    while (!stateStack_.empty()) {
        int state = stateStack_.back();

        for (std::size_t i = 0; i < 3; ++i) {
            std::string nonTerminal = syncNonTerminals[i];
            int resumeState = table_.gotoState(state, nonTerminal);
            if (resumeState == -1) {
                continue;
            }

            // discard input until the resume state can act on the lookahead
            while (true) {
                const Token &lookahead = tokens.peek();
                ActionEntry next = table_.action(resumeState, terminalOf(lookahead));
                if (next.kind != ActionEntry::Kind::Error) {
                    stateStack_.push_back(resumeState);
                    nodeStack_.push_back(new RuleNode(nonTerminal, std::vector<ParseNode *>()));
                    std::cout << "  recovered: resumed on " << nonTerminal
                        << " at state " << resumeState << "\n";
                    return true;
                }
                if (lookahead.type == TokenType::End) {
                    break;
                }
                tokens.advance();
            }
        }

        // state cannot resynchronise
        // drop its symbol and try the one below
        stateStack_.pop_back();
        if (!nodeStack_.empty()) {
            delete nodeStack_.back();
            nodeStack_.pop_back();
        }
    }

    return false;
}

bool Parser::parse(TokenStream &tokens) {
    // start fresh so the parser can be reused
    stateStack_.clear();
    freeNodes();
    stateStack_.push_back(0);

    std::cout << "\nParsing:\n";

    while (true) {
        int currentState = stateStack_.back();
        const Token &lookahead = tokens.peek();
        std::string terminal = terminalOf(lookahead);

        ActionEntry entry = table_.action(currentState, terminal);

        if (entry.kind == ActionEntry::Kind::Shift) {
            std::cout << "  shift '" << terminal << "' -> state " << entry.value << "\n";
            stateStack_.push_back(entry.value);
            nodeStack_.push_back(new TerminalNode(lookahead));
            tokens.advance();

        } else if (entry.kind == ActionEntry::Kind::Reduce) {
            const Production &production = productions_[entry.value];
            std::size_t rhsLength = production.rightHandSide.size();

            std::vector<ParseNode *> children;
            for (std::size_t i = nodeStack_.size() - rhsLength; i < nodeStack_.size(); ++i) {
                children.push_back(nodeStack_[i]);
            }

            // pop the right-hand side off both stacks
            for (std::size_t i = 0; i < rhsLength; ++i) {
                stateStack_.pop_back();
                nodeStack_.pop_back();
            }

            // move to the GOTO state for the rule left-hand side
            int exposedState = stateStack_.back();
            int gotoTarget = table_.gotoState(exposedState, production.leftHandSide);
            if (gotoTarget == -1) {
                std::cerr << "Internal error: no GOTO for " << production.leftHandSide
                    << " in state " << exposedState << "\n";
                for (std::size_t i = 0; i < children.size(); ++i) {
                    delete children[i];
                }

                freeNodes();

                std::cout << "\nResult: input REJECTED\n";
                return false;
            }
            stateStack_.push_back(gotoTarget);
            nodeStack_.push_back(new RuleNode(production.leftHandSide, children));

            std::cout << "  reduce by rule " << entry.value << ": ";
            printProduction(production);
            std::cout << "\n";

        } else if (entry.kind == ActionEntry::Kind::Accept) {
            if (errors_.hasErrors()) {
                std::cout << "\nResult: input REJECTED (" << errors_.count()
                    << " syntax error(s))\n";
                errors_.print();
                freeNodes();
                return false;
            }

            std::cout << "  accept\n";
            std::cout << "\nResult: input ACCEPTED\n";

            std::cout << "\nParse tree:\n";
            if (!nodeStack_.empty()) {
                ParseNode *root = nodeStack_.back();
                nodeStack_.pop_back();
                root->print(0);
                delete root;
            }

            freeNodes();

            return true;

        } else {
            // syntax error:
            // report it
            // then try to recover and keep parsing
            errors_.report(lookahead, std::string("unexpected ") + tokenTypeName(lookahead.type));
            std::cout << "  error at line " << lookahead.lineNumber
                << ", column " << lookahead.columnNumber
                << ": unexpected " << tokenTypeName(lookahead.type)
                << " '" << lookahead.lexeme << "'\n";

            if (errors_.count() > 50 || !recover(tokens)) {
                std::cout << "\nResult: input REJECTED (" << errors_.count()
                    << " syntax error(s))\n";
                errors_.print();
                freeNodes();
                return false;
            }

            // recovered: continue parsing from the resynchronised state
        }
    }
}