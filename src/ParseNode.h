//
// Created by thari on 8/06/2026.
//

#ifndef LR_PARSER_PARSENODE_H
#define LR_PARSER_PARSENODE_H

#include <functional>

/**
 * base class for parse tree nodes
 */
class ParseNode {
public:
    virtual ~ParseNode();

    virtual void print(int depth) const = 0;

    // check terminal nodes
    virtual bool isTerminal() const { return false; }

    // higher order traversal: apply visit to this node and every descendant
    void forEach(const std::function<void(const ParseNode &)> &visit) const {
        visit(*this);
        forEachChild(visit);
    }

protected:
    virtual void forEachChild(const std::function<void(const ParseNode &)> &) const {}
};



#endif //LR_PARSER_PARSENODE_H
