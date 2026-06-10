//
// Created by thari on 8/06/2026.
//

#ifndef LR_PARSER_TOKENSTREAM_H
#define LR_PARSER_TOKENSTREAM_H

#include <string>
#include <vector>

#include "Token.h"

class TokenStream {
public:
    // read token output file
    bool loadFromFile(const std::string &filePath);

    // print tokens
    void print() const;

    // get current token without consuming
    const Token &peek() const;

    // move to the next token
    void advance();

    // if end token
    bool atEnd() const;

    // get number of tokens
    std::size_t size() const;

private:
    std::vector<Token> tokens_;
    std::size_t position_ = 0;
};



#endif //LR_PARSER_TOKENSTREAM_H
