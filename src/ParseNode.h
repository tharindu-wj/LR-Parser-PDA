//
// Created by thari on 8/06/2026.
//

#ifndef LR_PARSER_PARSENODE_H
#define LR_PARSER_PARSENODE_H


/**
 * base class for parse tree nodes
 */
class ParseNode {
public:
    virtual ~ParseNode();

    virtual void print(int depth) const = 0;
};



#endif //LR_PARSER_PARSENODE_H
