//
// Created by thari on 7/06/2026.
//

#include <iostream>
#include <fstream>
#include <string>

#include "Grammar.h"
#include "FirstFollow.h"
#include "TokenStream.h"
#include "LrAutomaton.h"
#include "ParseTable.h"
#include "Parser.h"

int main(int argc, char* argv[]) {
    // validate arguments
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <grammar-file> [token-file]\n";
        return 1;
    }

    // load grammar from a file
    Grammar grammar;
    if (!grammar.load(argv[1])) {
        std::cerr << "Error: could not load grammar '" << argv[1] << "'.\n";
        return 1;
    }
    grammar.print();

    //
    FirstFollow firstFollow;
    firstFollow.compute(grammar);
    firstFollow.print(grammar);

    // build LR(0) automaton: PDA states
    LrAutomaton automaton;
    automaton.build(grammar);
    automaton.print();

    // build SLR(1): ACTION/GOTO tables
    ParseTable parseTable;
    parseTable.build(grammar, firstFollow, automaton);
    parseTable.print();

    // load token file
    if (argc >= 3) {
        TokenStream tokens;
        if (!tokens.loadFromFile(argv[2])) {
            std::cerr << "Error: could not open token file '" << argv[2] << "'.\n";
            return 1;
        }
        tokens.print();

        // PDA parser with token stream
        Parser parser(parseTable, automaton.getAugmentedProductions());
        parser.parse(tokens);
    }

    return 0;
}