# C-to-Mips-compiler
Compiler for C90 standard combined with basic python translator. This project was build as coursework and adheres to specs specified [here](https://github.com/LangProc/langproc-2019-cw). 

This compiler was built from scratch with the help of lexing tool flex and parsing tool bison. Features implemented should be robust enough to not contain any common bugs.

This project uses variants and thus requires newer version of flex and bison !!!

## Translator features

This project supports all translation features provided [here](https://github.com/LangProc/langproc-2019-cw/blob/master/c_translator_templates.md)

## C90 features supported

* local variables
* arithmetic and logical expressions
* if-then-else statements
* loops
* functions
* arrays (only `int` type)
* reading and writing elements of an array
* recursive function calls
* the `enum` keyword
* `switch` statements
* the `break` and `continue` keywords
* variables  of `double`, `float`, `int`, `unsigned` and pointer types
* Scopes.
* the `typedef` keyword (only basic ones)
* the `sizeof(...)` function 
* taking the address of a variable using the `&` operator
* dereferencing a pointer-variable using the `*` operator
* pointer arithmetic

# Build instructions 

Both translator and compiler are built using:
    make bin/c_compiler
    
The translator function is invoked using the flag `--translate`, with the source file and output file specified on the command line:

    bin/c_compiler --translate [source-file.c] -o [dest-file.py]
    
The compilation function is invoked using the flag `-S`, with the source file and output file specified on the command line:

    bin/c_compiler -S [source-file.c] -o [dest-file.s]
