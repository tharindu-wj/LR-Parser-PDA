//
// Created by thari on 8/06/2026.
//

#ifndef LR_PARSER_TERMINALNODE_H
#define LR_PARSER_TERMINALNODE_H

#include "ParseNode.h"
#include "Token.h"

/**
 *
 */
class TerminalNode:public ParseNode {
public:
    explicit TerminalNode(const Token &token);

    void print(int depth) const override;

private:
    Token token_;
};



#endif //LR_PARSER_TERMINALNODE_H
