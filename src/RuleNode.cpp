//
// Created by thari on 8/06/2026.
//

#include "RuleNode.h"

#include <iostream>

RuleNode::RuleNode(const std::string &ruleName, const std::vector<ParseNode *> &children)
    : ruleName_(ruleName), children_(children) {
}

RuleNode::~RuleNode() {
    for (std::size_t i = 0; i < children_.size(); ++i) {
        delete children_[i];
    }
}

void RuleNode::print(int depth) const {
    std::cout << std::string(depth * 2, ' ') << ruleName_ << "\n";
    for (std::size_t i = 0; i < children_.size(); ++i) {
        children_[i]->print(depth + 1);
    }
}