%option noyywrap

O   [0-7]
D   [0-9]
NZ  [1-9]
L   [a-zA-Z_]
A   [a-zA-Z_0-9]
H   [a-fA-F0-9]
HP  (0[xX])
E   ([Ee][+-]?{D}+)
P   ([Pp][+-]?{D}+)
FS  (f|F|l|L)
IS  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))
CP  (u|U|L)
SP  (u8|u|U|L)
ES  (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))
WS  [ \t\v\n\f]
ALL [\n]

%{

// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

#include <stdio.h>
#include "parser.tab.hpp"
#include <string>
#include<iostream>
# define YY_DECL yy::parser::symbol_type yylex ()
extern void yyerror(const char *);  /* prints grammar violation message */

extern int sym_type(const char *);  /* returns type from symbol table */

#define sym_type(identifier) IDENTIFIER /* with no symbol table, fake it */


%}

%%

"/*"(.|[\r\n])*?"*/"                                   { }
"//".*                                    { /* consume //-comment */ }

"break"					{ return yy::parser::make_BREAK(); }
"case"					{ return yy::parser::make_CASE(); }
"char"					{ return yy::parser::make_TYPE("char"); }
"continue"				{ return yy::parser::make_CONTINUE(); }
"default"		        { return yy::parser::make_DEFAULT(); }
"do"					{ return yy::parser::make_DO(); }
"double"				{ return yy::parser::make_TYPE("double"); }
"else"					{ return yy::parser::make_ELSE(); }
"enum"					{ return yy::parser::make_ENUM(); }
"float"					{ return yy::parser::make_TYPE("float"); }
"for"					{ return yy::parser::make_FOR(); }
"if"					{ return yy::parser::make_IF(); }
"int"					{ return yy::parser::make_TYPE("int"); }
"signed"                { return yy::parser::make_TYPE("int"); }
"signed int"            { return yy::parser::make_TYPE("int"); }
"return"				{ return yy::parser::make_RETURN(); }
"sizeof"				{ return yy::parser::make_SIZEOF(); }
"struct"				{ return yy::parser::make_STRUCT(); }
"switch"				{ return yy::parser::make_SWITCH(); }
"typedef"				{ return yy::parser::make_TYPEDEF(); }
"unsigned"				{ return yy::parser::make_TYPE("unsigned"); }
"unsigned int"          { return yy::parser::make_TYPE("unsigned"); }
"void"					{ return yy::parser::make_TYPE("void"); }
"while"					{ return yy::parser::make_WHILE(); }

{L}{A}*					{ std::string s(yytext); return yy::parser::make_IDENTIFIER(s); }

{HP}{H}+{IS}?				{ return yy::parser::make_CONSTANT(std::stol(yytext,0,16)); }

{NZ}{D}*{IS}?				{ return yy::parser::make_CONSTANT(std::stol(yytext)); }
"0"{O}*{IS}?				{ return yy::parser::make_CONSTANT(std::stol(yytext,0,8)); }
{CP}?"'"([^'\\\n]|{ES})+"'"		{ return yy::parser::make_CONSTANT(std::stol(yytext));}

{D}+{E}{FS}				{ return yy::parser::make_FCONSTANT(std::stof(yytext)); }
{D}*"."{D}+{E}?{FS}			{ return yy::parser::make_FCONSTANT(std::stof(yytext)); }
{D}+"."{E}?{FS}			{ return yy::parser::make_FCONSTANT(std::stof(yytext)); }
{HP}{H}*"."{H}+{P}{FS}		{ return yy::parser::make_FCONSTANT(std::stof(yytext)); }
{HP}{H}+"."{P}{FS}		{ return yy::parser::make_FCONSTANT(std::stof(yytext));  }

{D}+{E}			{ return yy::parser::make_DCONSTANT(std::stod(yytext)); }
{D}*"."{D}+{E}?		{ return yy::parser::make_DCONSTANT(std::stod(yytext)); }
{D}+"."{E}?	{ return yy::parser::make_DCONSTANT(std::stod(yytext)); }
{HP}{H}*"."{H}+{P}	{ return yy::parser::make_DCONSTANT(std::stod(yytext)); }
{HP}{H}+"."{P}		{ return yy::parser::make_DCONSTANT(std::stod(yytext));  }

({SP}?\"([^"\\\n]|{ES})*\"{WS}*)+	{ /*return STRING_LITERAL; */}

">>="					{ return yy::parser::make_RIGHT_ASSIGN(); }
"<<="					{ return yy::parser::make_LEFT_ASSIGN(); }
"+="					{ return yy::parser::make_ADD_ASSIGN(); }
"-="					{ return yy::parser::make_SUB_ASSIGN(); }
"*="					{ return yy::parser::make_MUL_ASSIGN(); }
"/="					{ return yy::parser::make_DIV_ASSIGN(); }
"%="					{ return yy::parser::make_MOD_ASSIGN(); }
"&="					{ return yy::parser::make_AND_ASSIGN(); }
"^="					{ return yy::parser::make_XOR_ASSIGN(); }
"|="					{ return yy::parser::make_OR_ASSIGN(); }
">>"					{ return yy::parser::make_RIGHT_OP(); }
"<<"					{ return yy::parser::make_LEFT_OP(); }
"++"					{ return yy::parser::make_INC_OP(); }
"--"					{ return yy::parser::make_DEC_OP(); }
"->"					{ return yy::parser::make_PTR_OP(); }
"&&"					{ return yy::parser::make_AND_OP(); }
"||"					{ return yy::parser::make_OR_OP(); }
"<="					{ return yy::parser::make_LE_OP(); }
">="					{ return yy::parser::make_GE_OP(); }
"=="					{ return yy::parser::make_EQ_OP(); }
"!="					{ return yy::parser::make_NE_OP(); }
";"					{ return yy::parser::make_SEMICOLON(); }
("{"|"<%")				{ return yy::parser::make_LCBRACKET();  }
("}"|"%>")				{ return yy::parser::make_RCBRACKET();  }
","					{ return yy::parser::make_COMMA();  }
":"					{ return yy::parser::make_COLON();  }
"="					{ return yy::parser::make_EQUAL();  }
"("					{ return yy::parser::make_LBRACKET(); }
")"					{ return yy::parser::make_RBRACKET();  }
("["|"<:")				{ return yy::parser::make_LSBRACKET();  }
("]"|":>")				{ return yy::parser::make_RSBRACKET();  }
"."					{ return yy::parser::make_DOT();  }
"&"					{ return yy::parser::make_BAND();  }
"!"					{ return yy::parser::make_EXCLAMATION();  }
"~"					{return yy::parser::make_TILDE(); }
"-"					{ return yy::parser::make_MINUS();}
"+"					{ return yy::parser::make_PLUS(); }
"*"					{ return yy::parser::make_MULTIPLY();}
"/"					{ return yy::parser::make_DIVIDE(); }
"%"					{  return yy::parser::make_MODULO(); }
"<"					{  return yy::parser::make_LT_OP(); }
">"					{ return yy::parser::make_GT_OP(); }
"^"					{  return yy::parser::make_BXOR();}
"|"					{  return yy::parser::make_BOR(); }
"?"					{  return yy::parser::make_QUESTION(); }

{WS}+					{ /* whitespace separates tokens */ }
.					{ /* discard bad characters */ }
"\n"            {}
%%



