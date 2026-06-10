cmake_minimum_required(VERSION 3.20)
project(lr_parser CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# enable strict warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
endif()

# size optimisation
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "-Os -s")
endif()

# addressSanitizer for the memory profiling
#   cmake -S parser -B parser/build -DENABLE_ASAN=ON
option(ENABLE_ASAN "Build with AddressSanitizer" OFF)

if(ENABLE_ASAN AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fsanitize=address -g)
    add_link_options(-fsanitize=address)
endif()

add_executable(lr_parser src/main.cpp src/Grammar.cpp src/TokenStream.cpp src/FirstFollow.cpp src/LrAutomaton.cpp
        src/ParseTable.cpp src/Parser.cpp src/ParseNode.cpp src/TerminalNode.cpp src/RuleNode.cpp
        src/ErrorReporter.cpp)
