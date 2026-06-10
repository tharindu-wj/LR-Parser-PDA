//
// Created by thari on 8/06/2026.
//

#include "TerminalNode.h"

#include <iostream>
#include <string>

TerminalNode::TerminalNode(const Token &token) : token_(token) {
}

void TerminalNode::print(int depth) const {
    std::cout << std::string(depth * 2, ' ') << "\"" << token_.lexeme << "\"\n";
}
