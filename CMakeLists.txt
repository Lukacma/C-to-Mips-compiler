cmake_minimum_required(VERSION 3.10)

project(Compiler)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_CXX_STANDARD 11)
set(GCC_FLAGS " -O3")
find_package(BISON)
find_package(FLEX)

include_directories(${PROJECT_SOURCE_DIR}/Parser/ast ${PROJECT_SOURCE_DIR}/Compiler/include ${PROJECT_SOURCE_DIR}/Translator ${CMAKE_CURRENT_BINARY_DIR}) 
bison_target(parser ${CMAKE_SOURCE_DIR}/Parser/src/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp)
flex_target(lexer ${CMAKE_SOURCE_DIR}/Lexer/lexer.flex  ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.cpp)
set (TRANSLATOR_SOURCE  ${CMAKE_SOURCE_DIR}/Translator/translator.cpp)
set(COMPILER_SOURCE  ${CMAKE_SOURCE_DIR}/Compiler/src/compile.cpp)
add_definitions(${GCC_FLAGS})
add_executable(translator ${TRANSLATOR_SOURCE} ${BISON_parser_OUTPUTS} ${FLEX_lexer_OUTPUTS})
add_executable(compiler ${COMPILER_SOURCE} ${BISON_parser_OUTPUTS} ${FLEX_lexer_OUTPUTS})