
%language "c++"
%define api.value.type variant
%define api.token.constructor
%define parse.error verbose
%require "3.2"
%code requires{
  #include "ast_expression.h"
  #include "ast_operators.h"
  #include "ast_primitives.h"
  #include "ast_unary.h"
  #include "ast_function.h"
  #include "ast_statement.h"
  #include "ast_initializer.h"
  #include<string>
  #include<iostream>
  #include<vector>
  #include <typeinfo>
  extern std::vector< std::shared_ptr<Expression>> groot;
  std::vector< std::shared_ptr<Expression>>& parseAST();

 
}
%code provides{
 yy::parser::symbol_type yylex ();

}


%token IDENTIFIER CONSTANT SEMICOLON LCBRACKET RCBRACKET COMMA COLON EQUAL LBRACKET RBRACKET LSBRACKET RSBRACKET FCONSTANT DCONSTANT
%token DOT BAND  EXCLAMATION TILDE MINUS PLUS MULTIPLY DIVIDE MODULO LT_OP GT_OP BXOR BOR QUESTION 
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN 
%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN
%token DOUBLE FLOAT ENUM CHAR SIGNED SIZEOF STRUCT TYPEDEF UNSIGNED VOID  

%token TYPE

%type<  std::shared_ptr<Expression>> primary_expression postfix_expression unary_expression multiplicative_expression additive_expression
%type<  std::shared_ptr<Expression>> shift_expression relational_expression equality_expression and_expression
%type<  std::shared_ptr<Expression>> exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type<  std::shared_ptr<Expression>> var_initializer conditional_expression expression   function_definition
%type<  std::shared_ptr<Expression>> enumerator
%type< std::vector< std::shared_ptr<Expression>>> enumerator_list enum_specifier argument_expression_list declaration  external_declaration translation_unit
%type<std::shared_ptr<Statement>>  statement compound_statement  selection_statement iteration_statement jump_statement labeled_statement
%type<std::shared_ptr<ExpressionStatement>> expression_statement
%type< std::vector<std::shared_ptr<Statement>> > block_item block_item_list
%type<std::string> unary_operator assignment_operator type_specifier 
%type<std::shared_ptr<Declaration>> init_declarator declarator direct_declarator
%type<std::vector<std::shared_ptr<Declaration>>> init_declarator_list  parameter_list
%type<std::string>  IDENTIFIER TYPE 
%type<int>  pointer
%type<std::shared_ptr<Initializer>> initializer_list initializer
%type<float> FCONSTANT
%type<double> DCONSTANT
%type<long> CONSTANT
%start root


%%
primary_expression
	: IDENTIFIER        {$$=std::make_shared<Variable>($1);}
	| CONSTANT          {$$=std::make_shared<Constant>($1);}
	| FCONSTANT 		{$$=std::make_shared<FConstant>($1);}
	|DCONSTANT		{$$=std::make_shared<DConstant>($1);}
	| LBRACKET expression RBRACKET  {$$=$2;}


postfix_expression
    : primary_expression {$$=$1;}
	| postfix_expression LSBRACKET expression RSBRACKET {if(std::dynamic_pointer_cast<Declaration>($1)!=nullptr){
															$$=std::make_shared<ArrayCall>(std::static_pointer_cast<Declaration>($1)->getId(),$3);
															}
														 else{
														 	std::static_pointer_cast<ArrayCall>($1)->number.push_back($3);
															 $$=$1;
															 }}
    | postfix_expression LBRACKET RBRACKET	{std::vector< std::shared_ptr<Expression>> tmp={};
												$$=std::make_shared<FunctionCall>(std::dynamic_pointer_cast<Identifiable>($1)->getId(),tmp);}
	| postfix_expression LBRACKET argument_expression_list RBRACKET {$$=std::make_shared<FunctionCall>(std::dynamic_pointer_cast<Identifiable>($1)->getId(),$3);}
    | postfix_expression INC_OP {$$=std::make_shared<PostfixOperator>($1,"++");} 
    | postfix_expression DEC_OP {$$=std::make_shared<PostfixOperator>($1,"--");} 

argument_expression_list
	: expression {$$={$1};}
	| argument_expression_list COMMA expression {$$=$1;
												 $$.push_back($3);}

unary_expression
    : postfix_expression {$$=$1;}
    | INC_OP unary_expression {$$=std::make_shared<PrefixOperator>($2,"++");}
    | DEC_OP unary_expression {$$=std::make_shared<PrefixOperator>($2,"--");}
	| MULTIPLY unary_expression {$$=std::make_shared<Dereference>($2);}
    | unary_operator unary_expression {$$= std::make_shared<Unary>($2,$1);}
	| SIZEOF IDENTIFIER { $$=std::make_shared<SizeOf>($2);}
	| SIZEOF LBRACKET TYPE RBRACKET { $$=std::make_shared<SizeOf>($3);}
	| SIZEOF TYPE { $$=std::make_shared<SizeOf>($2);}
	| SIZEOF LBRACKET IDENTIFIER RBRACKET { $$=std::make_shared<SizeOf>($3);}	

unary_operator
	: BAND   {$$="&";}
	| PLUS   {$$="+";}
	| MINUS  {$$="-";}
	| TILDE   {$$="~";}
	| EXCLAMATION   {$$="!";}

multiplicative_expression
	: unary_expression   {$$=$1;}
	| multiplicative_expression MULTIPLY unary_expression    {$$=std::make_shared<BinaryOperator>($1,$3,"*");}
	| multiplicative_expression DIVIDE unary_expression    {$$=std::make_shared<BinaryOperator>($1,$3,"/");}
	| multiplicative_expression MODULO unary_expression    {$$=std::make_shared<BinaryOperator>($1,$3,"%");}  

additive_expression
	: multiplicative_expression {$$=$1;}
	| additive_expression PLUS multiplicative_expression  {$$=std::make_shared<BinaryOperator>($1,$3,"+");}
	| additive_expression MINUS multiplicative_expression  {$$=std::make_shared<BinaryOperator>($1,$3,"-");}

shift_expression
	: additive_expression   {$$=$1;}
	| shift_expression LEFT_OP additive_expression  {$$=std::make_shared<BinaryOperator>($1,$3,"<<");}
	| shift_expression RIGHT_OP additive_expression  {$$=std::make_shared<BinaryOperator>($1,$3,">>");}

relational_expression
	: shift_expression  {$$=$1;}
	| relational_expression LT_OP shift_expression    {$$=std::make_shared<BinaryOperator>($1,$3,"<");}
	| relational_expression GT_OP shift_expression    {$$=std::make_shared<BinaryOperator>($1,$3,">");}
	| relational_expression LE_OP shift_expression  {$$=std::make_shared<BinaryOperator>($1,$3,"<=");}
	| relational_expression GE_OP shift_expression  {$$=std::make_shared<BinaryOperator>($1,$3,">=");}

equality_expression
	: relational_expression {$$=$1;}
	| equality_expression EQ_OP relational_expression    {$$=std::make_shared<BinaryOperator>($1,$3,"==");}
	| equality_expression NE_OP relational_expression    {$$=std::make_shared<BinaryOperator>($1,$3,"!=");}

and_expression
	: equality_expression   {$$=$1;}
	| and_expression BAND equality_expression    {$$=std::make_shared<BinaryOperator>($1,$3,"&");}

exclusive_or_expression
	: and_expression    {$$=$1;}
	| exclusive_or_expression BXOR and_expression    {$$=std::make_shared<BinaryOperator>($1,$3,"^");}

inclusive_or_expression
	: exclusive_or_expression   {$$=$1;}
	| inclusive_or_expression BOR exclusive_or_expression   {$$=std::make_shared<BinaryOperator>($1,$3,"|");}

logical_and_expression
	: inclusive_or_expression   {$$=$1;}
	| logical_and_expression AND_OP inclusive_or_expression   {$$=std::make_shared<BinaryOperator>($1,$3,"&&");}

logical_or_expression
	: logical_and_expression    {$$=$1;}
	| logical_or_expression OR_OP logical_and_expression    {$$=std::make_shared<BinaryOperator>($1,$3,"||");}

conditional_expression
	: logical_or_expression {$$=$1;}
	| logical_or_expression QUESTION expression COLON conditional_expression {$$=std::make_shared<TernaryOperator>($1,$3,$5);}

expression
    : conditional_expression    {$$=$1;}
	| unary_expression assignment_operator expression {$$=std::make_shared<BinaryOperator>($1,$3,$2);}

assignment_operator
	: EQUAL   {$$="=";}
	| MUL_ASSIGN    {$$="*=";}
	| DIV_ASSIGN    {$$="/=";}
	| MOD_ASSIGN    {$$="%=";}
	| ADD_ASSIGN    {$$="+=";}
	| SUB_ASSIGN    {$$="-=";}
	| LEFT_ASSIGN   {$$="<<=";}
	| RIGHT_ASSIGN  {$$=">>=";}
	| AND_ASSIGN    {$$="&=";}
	| XOR_ASSIGN    {$$="^=";}
	| OR_ASSIGN     {$$="|=";}

declaration
	: type_specifier SEMICOLON {std::cerr<<"type specifier without declarator: probably not supported"<<std::endl; std::exit(1);}
	| enum_specifier SEMICOLON { $$ = $1;}
	| enum_specifier init_declarator_list SEMICOLON {

													$$={};

													for(auto el: $2){
														el->type="int";
														$$.push_back(el);
													}
													
													for(auto el: $1){
														
														$$.push_back(el);
													}
													}
	| type_specifier init_declarator_list SEMICOLON 	{
													$$={};
													for(auto el: $2){
														el->type=$1;
														$$.push_back(el);
													}
												}



init_declarator_list
	: init_declarator {$$={$1};}
	| init_declarator_list COMMA init_declarator {$1.push_back($3); $$=$1;}
	

init_declarator
	: declarator	{$$=$1;}
	| declarator EQUAL var_initializer	{if($1->classType=="pointer"){
											$$=std::make_shared<Pointer>(std::static_pointer_cast<Pointer>($1),$3);
											if(std::dynamic_pointer_cast<FuncPointer>($1)!=nullptr){
												int numOfParam=std::static_pointer_cast<FuncPointer>($1)->getParams();
												$$=std::make_shared<FuncPointer>(std::static_pointer_cast<Pointer>($$),numOfParam,$3);}
										}
										 	else
										 	$$=std::make_shared<Variable>("",$1->getId(),$3);}
	| declarator EQUAL LCBRACKET initializer_list RCBRACKET {std::static_pointer_cast<Array>($1)->init_list=$4;$$=$1;}
	| declarator EQUAL LCBRACKET initializer_list COMMA RCBRACKET {std::static_pointer_cast<Array>($1)->init_list=$4;$$=$1;}
	

type_specifier
    : TYPE {$$=$1;}
	;

declarator
	: pointer direct_declarator	{if(std::dynamic_pointer_cast<Function>($2)==nullptr)
									$$=std::make_shared<Pointer>($2->getId(),nullptr,$2->type,$1);
								else
									$$=$2;}
	| direct_declarator {$$=$1;}

direct_declarator
	: IDENTIFIER	{$$=std::make_shared<Variable>($1);}
	| LBRACKET declarator RBRACKET {$$=$2;}
	| direct_declarator LSBRACKET CONSTANT RSBRACKET	{if($1->classType=="var"){
													$$=std::make_shared<Array>($1->getId(),"int",$3);
												 }
												 else{
												 	std::static_pointer_cast<Array>($1)->dimensions.push_back($3);
													 $$=$1;
												 }
												}
	| direct_declarator LBRACKET parameter_list RBRACKET {if($1->classType=="pointer"){
															$$=std::make_shared<FuncPointer>(std::static_pointer_cast<Pointer>($1),$3.size());
															}
														  else
															$$=std::make_shared<Function>($1->getId(),nullptr,"",$3);}
	| direct_declarator LBRACKET RBRACKET {std::vector<std::shared_ptr<Declaration>> tmp={};
									$$=std::make_shared<Function>($1->getId(),nullptr,"",tmp);}


enum_specifier
	: ENUM LCBRACKET enumerator_list RCBRACKET {$$ = $3;}
	| ENUM LCBRACKET enumerator_list COMMA RCBRACKET {$$ = $3;}
	| ENUM IDENTIFIER LCBRACKET enumerator_list RCBRACKET {$$ = $4;}
	| ENUM IDENTIFIER LCBRACKET enumerator_list COMMA RCBRACKET {$$ = $4;}
	| ENUM IDENTIFIER {$$={};}
	;

enumerator_list
	: enumerator { 
		auto tmp = std::dynamic_pointer_cast<Variable>($1);
		if (tmp->getValue() == nullptr) 
		{ 
			tmp->setValue(std::make_shared<Constant>( 0));
		}
		$$.push_back(tmp); }
	| enumerator_list COMMA enumerator { $$=$1;
										auto en = std::dynamic_pointer_cast<Variable>($3);
										if (en->getValue() == nullptr)
										{
											auto last = std::dynamic_pointer_cast<Variable>($$.back());
											auto tmp = std::dynamic_pointer_cast<Constant>(last->getValue());
											en->setValue(std::make_shared<Constant>( tmp->getValue()+1));
										} 
										$$.push_back(en);
										}
	;

enumerator
	: IDENTIFIER EQUAL CONSTANT {
								auto num = std::make_shared<Constant>( $3);
								auto tmp = std::make_shared<Variable>("int", $1, num);
								$$ = std::static_pointer_cast<Expression>(tmp);}
								
	| IDENTIFIER {	auto tmp = std::make_shared<Variable>("int", $1, nullptr);
					$$ = std::static_pointer_cast<Expression>(tmp);}
	;

pointer
	: MULTIPLY	{$$=1;}
	| MULTIPLY pointer {$$=$2+1;}

parameter_list
	: type_specifier declarator { $2->type=$1;$$={$2};}
	| parameter_list COMMA type_specifier declarator{$$=$1;
													$4->type=$3;
													$$.push_back($4);}

var_initializer
	: expression {$$=$1;}
	| LCBRACKET CONSTANT RCBRACKET {$$=std::make_shared<Constant>($2);}
	| LCBRACKET MINUS CONSTANT RCBRACKET {$$=std::make_shared<Constant>(-$3);}

initializer
	: CONSTANT	{$$=std::make_shared<Initializer>($1);}
	| MINUS CONSTANT	{$$=std::make_shared<Initializer>(-$2);}
	| LCBRACKET initializer_list RCBRACKET	{$$=$2;}
	| LCBRACKET initializer_list COMMA RCBRACKET {$$=$2;}

initializer_list
	: initializer { std::vector<std::shared_ptr<Initializer>> tmp={$1};
					$$=std::make_shared<Initializer>(tmp,true);}
	| initializer_list COMMA initializer {$$=$1;
										  $$->init.push_back($3);}

statement
	: compound_statement	{$$=$1;}
	| expression_statement	{$$=$1;}
	| selection_statement	{$$=$1;}
	| iteration_statement	{$$=$1;}
	| jump_statement	{$$=$1;}
	| labeled_statement {$$=$1;}

labeled_statement: CASE expression COLON statement	{$$=std::make_shared<CaseStatement>($2,$4);}
	| DEFAULT COLON statement	{$$=std::make_shared<CaseStatement>($3);}

compound_statement
	: LCBRACKET RCBRACKET	{$$=std::make_shared<CompoundStatement>();}
	| LCBRACKET block_item_list RCBRACKET	{$$=std::make_shared<CompoundStatement>($2);}

block_item_list
	: block_item {$$=$1;}
	| block_item_list block_item	{$$=$1;
									 $$.insert($$.end(),$2.begin(),$2.end());
									}	

block_item
	: declaration 	{$$={};
					 for(auto el:$1){
						$$.push_back(std::make_shared<ExpressionStatement>(el));
					 } 
					}
	| statement	{$$={$1};}

expression_statement
	: SEMICOLON	{$$=std::make_shared<ExpressionStatement>();}
	| expression SEMICOLON	{$$=std::make_shared<ExpressionStatement>($1);}

selection_statement
	: IF LBRACKET expression RBRACKET statement {$$=std::make_shared<IfStatement>($3,$5);}
	| IF LBRACKET expression RBRACKET statement ELSE statement	{$$=std::make_shared<IfStatement>($3,$5,$7);}
	| SWITCH LBRACKET expression RBRACKET statement	{$$=std::make_shared<SwitchStatement>($3,$5);}

iteration_statement
	: WHILE LBRACKET expression RBRACKET statement	{$$= std::make_shared<WhileStatement>($3,$5);}
	| FOR LBRACKET expression_statement expression_statement RBRACKET statement {$$= std::make_shared<ForStatement>($3,$4,nullptr,$6);}
	| FOR LBRACKET expression_statement expression_statement expression RBRACKET statement {$$= std::make_shared<ForStatement>($3,$4,$5,$7);}

jump_statement: RETURN SEMICOLON		{$$=std::make_shared<ReturnStatement>();}
	| CONTINUE SEMICOLON	{$$=std::make_shared<ContinueStatement>();}
	| BREAK SEMICOLON		{$$=std::make_shared<BreakStatement>();}
	| RETURN expression SEMICOLON	{$$=std::make_shared<ReturnStatement>($2);}
	

translation_unit
	: external_declaration	{$$=$1;}
	| translation_unit external_declaration {$$=$1;
									 		 $$.insert($$.end(),$2.begin(),$2.end());
											}	
	;

external_declaration
	: function_definition {$$={$1};}
	| declaration	{$$=$1;}
	;

function_definition
	: type_specifier declarator compound_statement {
													auto tmp=std::dynamic_pointer_cast<Function>($2);
													$$=std::make_shared<Function>(tmp->getId(),$3,$1,tmp->getParams());}
root: translation_unit {groot=$1;}
%%

void yy::parser::error (const std::string& m)
{
  std::cerr  << m << '\n';
}
std::vector< std::shared_ptr<Expression>> groot;
int unique_num = 0;
std::vector<std::shared_ptr<Expression>>& parseAST(){
	groot={};
	yy::parser par;
    int ret=par();
    
    return groot;
}