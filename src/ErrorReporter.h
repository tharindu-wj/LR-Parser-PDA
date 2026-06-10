//
// Created by thari on 10/06/2026.
//

#ifndef LR_PARSER_ERRORREPORTER_H
#define LR_PARSER_ERRORREPORTER_H

#include <string>
#include <vector>

#include "Token.h"

class ErrorReporter {
public:
    // record one error at the position of the given token
    void report(const Token &token, const std::string &message);

    bool hasErrors() const;

    std::size_t count() const;

    // print all recorded error followed by a total count
    void print() const;

private:
    struct Entry {
        int line;
        int column;
        std::string lexeme;
        std::string message;
    };

    std::vector<Entry> errors_;
};



#endif //LR_PARSER_ERRORREPORTER_H
