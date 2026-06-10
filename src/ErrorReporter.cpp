//
// Created by thari on 10/06/2026.
//

#include "ErrorReporter.h"

#include <iostream>

void ErrorReporter::report(const Token &token, const std::string &message) {
    Entry entry;
    entry.line = token.lineNumber;
    entry.column = token.columnNumber;
    entry.lexeme = token.lexeme;
    entry.message = message;
    errors_.push_back(entry);
}

bool ErrorReporter::hasErrors() const {
    return !errors_.empty();
}

std::size_t ErrorReporter::count() const {
    return errors_.size();
}

void ErrorReporter::print() const {
    for (std::size_t i = 0; i < errors_.size(); ++i) {
        const Entry &entry = errors_[i];
        std::cout << "  line " << entry.line << ", column " << entry.column
            << ": " << entry.message << " '" << entry.lexeme << "'\n";
    }
    std::cout << errors_.size() << " syntax error(s) found.\n";
}