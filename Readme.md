# LR Parser using PDA and SLR(1)

This program is a table-driven LR parser for a simplified object-oriented language. This program built as a deterministic pushdown automaton using the SLR(1) method.
This program reads a token file, checks it against a context-free grammar, and prints a parse tree when the input
is valid. Also, this reports syntax errors with their line and column when the input is invalid.

## Requirements
- A C++17 compiler (GCC, Clang, or MSVC)
- CMake 3.20 or newer

## Build
From the project root (the folder containing `CMakeLists.txt`):


> CLion: open this folder with CLion which detects `CMakeLists.txt` and configures automatically.
> You then only need to set the run arguments (see below).

## Run
The parser takes a grammar file and a token file:

    lr_parser <grammar-file> <token-file>

Example (a valid class):

    lr_parser grammar/grammar_v2.txt tests/inputs/A2.txt