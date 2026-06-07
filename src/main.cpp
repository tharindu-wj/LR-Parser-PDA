//
// Created by thari on 7/06/2026.
//

#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    // validate input file
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input-file>\n";
        return 1;
    }

    // open input file.
    std::ifstream input(argv[1]);
    if (!input) {
        std::cout << "Error: could not open file '" << argv[1] << "'.\n";
        return 1;
    }

    // read input file
    std::string line;
    int line_no = 1;
    while (std::getline(input, line)) {
        std::cout << line_no << ": " << line << '\n';
        ++line_no;
    }

    return 0;
}