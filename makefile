CPPFLAGS += -std=c++11 -O3
INCLUDE = Working Parser/ast Compiler/include Translator
CPPFLAGS += $(patsubst %, -I%, $(INCLUDE))

all : bin/print_canonical bin/eval_expr

Working/parser.tab.cpp Working/parser.tab.hpp : Parser/src/parser.y
	mkdir -p Working
	bison -v -d Parser/src/parser.y -o Working/parser.tab.cpp

Working/lexer.yy.cpp : Lexer/lexer.flex Working/parser.tab.hpp
	mkdir -p Working
	flex -o Working/lexer.yy.cpp  Lexer/lexer.flex


parser : Working/lexer.yy.cpp Working/parser.tab.cpp
	mkdir -p bin
	g++  -c $(CPPFLAGS) -o Working/lexer.o Working/lexer.yy.cpp
	g++  -c $(CPPFLAGS) -o Working/parser.o Working/parser.tab.cpp

driver: parser
	mkdir -p bin
	g++ -c $(CPPFLAGS) Translator/translator.cpp -o Working/translator.o
	g++ $(CPPFLAGS) Working/lexer.o Working/parser.o Working/translator.o -o bin/translator


compiler: parser
	mkdir -p bin
	g++ -c $(CPPFLAGS) Compiler/src/compile.cpp -o Working/compile.o
	g++ $(CPPFLAGS) Working/lexer.o Working/parser.o Working/compile.o -o bin/compiler

clean :
	@-rm -r Working
	@-rm -r tmp

bin/c_compiler: driver compiler clean
	chmod u+x bin/c_compiler
	