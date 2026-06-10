//
// Created by thari on 7/06/2026.
//

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

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

    // time: table construction
    auto buildStart = std::chrono::steady_clock::now();

    // load grammar from a file
    Grammar grammar;
    if (!grammar.load(argv[1])) {
        std::cerr << "Error: could not load grammar '" << argv[1] << "'.\n";
        return 1;
    }
    // grammar.print();

    //
    FirstFollow firstFollow;
    firstFollow.compute(grammar);
    // firstFollow.print(grammar);

    // build LR(0) automaton: PDA states
    LrAutomaton automaton;
    automaton.build(grammar);
    // automaton.print();

    // build SLR(1): ACTION/GOTO tables
    ParseTable parseTable;
    parseTable.build(grammar, firstFollow, automaton);
    auto buildEnd = std::chrono::steady_clock::now();
    // parseTable.print();

    // load token file
    if (argc >= 3) {
        TokenStream tokens;
        if (!tokens.loadFromFile(argv[2])) {
            std::cerr << "Error: could not open token file '" << argv[2] << "'.\n";
            return 1;
        }
        // tokens.print();

        // PDA parser with token stream
        Parser parser(parseTable, automaton.getAugmentedProductions());
        auto parseStart = std::chrono::steady_clock::now();
        parser.parse(tokens);
        auto parseEnd = std::chrono::steady_clock::now();

        using ms = std::chrono::duration<double, std::milli>;
        double buildMs = ms(buildEnd - buildStart).count();
        double wallMs = ms(parseEnd - parseStart).count();
        double loopMs = parser.lastParseMs();
        std::cerr << "\n--- timing ---\n";
        std::cerr << "Table build: " << buildMs << " ms (one-time)\n";
        std::cerr << "Parse loop:  " << loopMs << " ms\n";
        std::cerr << "Tree emit:   " << (wallMs - loopMs) << " ms\n";
        std::cerr << "Parse total: " << wallMs << " ms\n";
    }

    return 0;
}