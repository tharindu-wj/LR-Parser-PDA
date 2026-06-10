//
// Created by thari on 8/06/2026.
//

#ifndef LR_PARSER_RULENODE_H
#define LR_PARSER_RULENODE_H

#include <string>
#include <vector>

#include "ParseNode.h"

/**
 *
 */
class RuleNode:public ParseNode {
public:
    RuleNode(const std::string &ruleName, const std::vector<ParseNode *> &children);

    ~RuleNode() override;

    void print(int depth) const override;

private:
    std::string ruleName_;
    std::vector<ParseNode *> children_;
};



#endif //LR_PARSER_RULENODE_H
