#ifndef AST_STATEMENT
#define AST_STATEMENT
#include <vector>
#include <iostream>
#include "ast_expression.h"
#include "ast_initializer.h"
#include <memory>

extern int unique_num;

class Statement
{
public:
    virtual void print() = 0;
    virtual void translate(int tabs, TranslatorContext &context) = 0;
    virtual std::string compile(Context &context) = 0;
};

typedef std::shared_ptr<Statement> StatementPtr;

class CompoundStatement : public Statement
{
private:
    std::vector<StatementPtr> _statements;

public:
    CompoundStatement(std::vector<StatementPtr> &statements) : _statements(statements){};
    CompoundStatement() : _statements({}){};
    const std::vector<StatementPtr> &getStatements() const
    {
        return _statements;
    }
    void print()
    {
        std::cout << " CompoundStatement: {" << std::endl;
        for (auto el : _statements)
        {
            el->print();
        }
        std::cout << "}" << std::endl;
    }
    void translate(int tabs, TranslatorContext &context)
    {
        for (auto el : _statements)
        {
            //std::cout << std::string(tabs, '\t');
            if (el != nullptr) el->translate(tabs, context);
        }
        std::cout << std::endl;
    }

    std::string compile(Context &context)
    {
        Context new_context(context);
        for (auto el : _statements)
        {
            el->compile(new_context);
        }
        if (context.caseflags.size() <= new_context.caseflags.size())
            context.caseflags = new_context.caseflags;

        return "";
    }
};

class ExpressionStatement : public Statement
{
private:
    ExpressionPtr _expression;

public:
    ExpressionStatement(ExpressionPtr exp) : _expression(exp){};
    ExpressionStatement() : _expression(nullptr){};
    ExpressionPtr getExpression() const
    {
        return _expression;
    }
    void print()
    {
        std::cout << "ExpressionStatement: ";
        _expression->print();
        std::cout << " ;" << std::endl;
    }

    void translate(int tabs, TranslatorContext &context)
    {
        std::cout << std::string(tabs, '\t');
        if (_expression != nullptr) _expression->translate(tabs, context);
        std::cout << std::endl;
    }
    std::string compile(Context &context)
    {
        std::string expType;
        if (_expression != nullptr)
        {
            expType = _expression->compile(context);
        }
        return expType;
    }
    bool is_empty()
    {
        return _expression == nullptr ? true : false;
    }
};

class IfStatement : public Statement
{
private:
    ExpressionPtr _condition;
    StatementPtr _holds;
    StatementPtr _else;

public:
    IfStatement(ExpressionPtr cond, StatementPtr holds, StatementPtr el) : _condition(cond), _holds(holds), _else(el){};
    IfStatement(ExpressionPtr cond, StatementPtr holds) : _condition(cond), _holds(holds), _else(NULL){};
    ExpressionPtr getCondition() const
    {
        return _condition;
    }
    StatementPtr getHolds() const
    {
        return _holds;
    }
    StatementPtr getElse() const
    {
        return _else;
    }
    void print()
    {
        std::cout << "SelectionStatement: If( ";
        _condition->print();
        std::cout << " )" << std::endl;
        std::cout << "{ " << std::endl;
        _holds->print();
        std::cout << " }" << std::endl;
        if (_else != nullptr)
        {
            std::cout << "else { " << std::endl;
            _else->print();
            std::cout << " }" << std::endl;
        }
    }
    void translate(int tabs, TranslatorContext &context)
    {
        std::cout << std::string(tabs, '\t') << "if (";
        _condition->translate(tabs, context);
        std::cout << "):" << std::endl;

        std::cout << std::string(tabs+1, '\t') << "pass" <<std::endl;

        if (_holds != nullptr)_holds->translate(tabs + 1, context);

        std::cout << std::endl;
        if (_else != nullptr)
        {
            
            std::cout << std::string(tabs, '\t') << "else:" << std::endl;
            std::cout << std::string(tabs+1, '\t') << "pass" <<std::endl;
            _else->translate(tabs + 1, context);
            std::cout << std::endl;
        }
    }

    std::string compile(Context &context)
    {
        std::string elselabel;
        std::string nextlabel = "C" + std::to_string(unique_num++);
        std::string conditionType = _condition->compile(context);
        if (_else != nullptr)
            elselabel = "EL" + std::to_string(unique_num++);
        else
            elselabel = nextlabel;

        if (conditionType == "float")
        {
            std::cout << "\tadd $2,$0,$0" << std::endl;
            std::cout << "\tmtc1 $2,$f4" << std::endl;
            std::cout << "\tcvt.s.w $f4,$f4" << std::endl;
            std::cout << "\t c.eq.s $f0,$f4" << std::endl;
            std::cout << "\t bc1t " << elselabel << std::endl;
        }
        else if (conditionType == "double")
        {
            std::cout << "\tadd $2,$0,$0" << std::endl;
            std::cout << "\tmtc1 $2,$f4" << std::endl;
            std::cout << "\tcvt.d.w $f4,$f4" << std::endl;
            std::cout << "\t c.eq.d $f0,$f4" << std::endl;
            std::cout << "\t bc1t " << elselabel << std::endl;
        }
        else
        {
            std::cout << "\t"
                      << "beq $2,$0," << elselabel << std::endl;
            std::cout << "\t"
                      << "nop" << std::endl;
        }
        _holds->compile(context);
        if (_else != nullptr)
        {
            std::cout << "\t"
                      << "b " << nextlabel << std::endl;
            std::cout << "\t"
                      << "nop" << std::endl;
            std::cout << elselabel << ":" << std::endl;
            _else->compile(context);
        }
        std::cout << nextlabel << ":" << std::endl;

        return "";
    }
};

class WhileStatement : public Statement
{
private:
    ExpressionPtr _condition;
    StatementPtr _body;

public:
    WhileStatement(ExpressionPtr cond, StatementPtr body) : _condition(cond), _body(body){};
    ExpressionPtr getCondition() const
    {
        return _condition;
    }
    StatementPtr getBody() const
    {
        return _body;
    }
    void print()
    {
        std::cout << "while (";
        _condition->print();
        std::cout << " )" << std::endl;
        std::cout << "{ " << std::endl;
        _body->print();
        std::cout << "} " << std::endl;
    }
    void translate(int tabs, TranslatorContext &context)
    {
        std::cout << std::string(tabs, '\t') << "while (";
        _condition->translate(tabs, context);
        std::cout << "): " << std::endl;
        std::cout << std::string(tabs+1, '\t') << "pass" <<std::endl;
        if (_body != nullptr) _body->translate(tabs + 1, context);
        std::cout << std::endl;
    }

    std::string compile(Context &context)
    {

        std::string whilelabel = "W" + std::to_string(unique_num++);
        std::string nextlabel = "C" + std::to_string(unique_num++);
        Context new_context(nextlabel, whilelabel, context);
        std::cout << whilelabel << ":" << std::endl;
        std::string conditionType = _condition->compile(context);
        if (conditionType == "float")
        {
            std::cout << "\tadd $2,$0,$0" << std::endl;
            std::cout << "\tmtc1 $2,$f4" << std::endl;
            std::cout << "\tcvt.s.w $f4,$f4" << std::endl;
            std::cout << "\t c.eq.s $f0,$f4" << std::endl;
            std::cout << "\t bc1t " << nextlabel << std::endl;
            std::cout << "\t nop" << std::endl;
        }
        else if (conditionType == "double")
        {
            std::cout << "\tadd $2,$0,$0" << std::endl;
            std::cout << "\tmtc1 $2,$f4" << std::endl;
            std::cout << "\tcvt.d.w $f4,$f4" << std::endl;
            std::cout << "\t c.eq.d $f0,$f4" << std::endl;
            std::cout << "\t bc1t " << nextlabel << std::endl;
            std::cout << "\t nop" << std::endl;
        }
        else
        {

            std::cout << "\t"
                      << "beq $2,$0," << nextlabel << std::endl;
            std::cout << "\t"
                      << "nop" << std::endl;
        }
        _body->compile(new_context);
        std::cout << "\t"
                  << "b " << whilelabel << std::endl;
        std::cout << "\t"
                  << "nop" << std::endl;
        std::cout << nextlabel << ":" << std::endl;

        return "";
    }
};

class ReturnStatement : public Statement
{
private:
    ExpressionPtr _expression;

public:
    ReturnStatement() : _expression(nullptr){};
    ReturnStatement(ExpressionPtr expr) : _expression(expr){};
    void print()
    {
        std::cout << "[ "
                  << "return ";
        if (_expression != nullptr)
            _expression->print();
        std::cout << " ;"
                  << "]" << std::endl;
    }

    void translate(int tabs, TranslatorContext &context)
    {
        std::cout << std::string(tabs, '\t') << "return ";
        if (_expression != nullptr)
            _expression->translate(tabs, context);
        std::cout << std::endl;
    }

    std::string compile(Context &context)
    {
        if (_expression != nullptr)
            _expression->compile(context);
        std::cout << "\t"
                  << "move $sp,$fp" << std::endl;
        std::cout << "\t"
                  << "lw $fp, 0($sp)" << std::endl;
        std::cout << "\t nop" << std::endl;
        std::cout << "\t"
                  << "lw $31, 4($sp)" << std::endl;
        std::cout << "\t nop" << std::endl;

        std::cout << "\t"
                  << "addiu $sp, $sp, 8" << std::endl;
        std::cout << "\t"
                  << "jr $31" << std::endl;
        std::cout << "\t nop" << std::endl;

        return "";
    }
};

class BreakStatement : public Statement
{
public:
    void print() override{};
    void translate(int tabs, TranslatorContext &context) override{};
    std::string compile(Context &context) override
    {
        std::cout << "\t"
                  << "b " << context.getBflag() << std::endl;
        std::cout << "\t"
                  << "nop" << std::endl;
        return "";
    }
};
class ContinueStatement : public Statement
{
public:
    void print() override{};
    void translate(int tabs, TranslatorContext &context) override{};
    std::string compile(Context &context) override
    {

        std::cout << "\t"
                  << "b " << context.getCflag() << std::endl;
        std::cout << "\t"
                  << "nop" << std::endl;

        return "";
    }
};
typedef std::shared_ptr<ExpressionStatement> ExpStatPtr;
class ForStatement : public Statement
{
private:
    ExpStatPtr _exp1, _exp2;
    ExpressionPtr _exp3;
    StatementPtr _body;

public:
    ForStatement(ExpStatPtr a, ExpStatPtr b, ExpressionPtr c, StatementPtr body) : _exp1(a), _exp2(b), _exp3(c), _body(body){};
    void print() override{};
    void translate(int tabs, TranslatorContext &context) override{};
    std::string compile(Context &context) override
    {
        /* I am using and realtion described in standard that states:
            for (exp1; exp2; exp3) body is equivalent to 
            exp1;
            while(exp2){
                body
                exp3 ;
            }*/
        std::string forlabel = "F" + std::to_string(unique_num++);
        std::string continuelabel = "CON" + std::to_string(unique_num++);
        std::string nextlabel = "C" + std::to_string(unique_num++);
        Context new_context(nextlabel, continuelabel, context);
        _exp1->compile(context);

        std::cout << forlabel << ":" << std::endl;
        std::string conditionType = "int";
        if (!_exp2->is_empty())
        {
            conditionType = _exp2->compile(context);
        }
        else
        {
            std::cout << "\t"
                      << "addi $2,$0,1" << std::endl;
        }
        if (conditionType == "float")
        {
            std::cout << "\tadd $2,$0,$0" << std::endl;
            std::cout << "\tmtc1 $2,$f4" << std::endl;
            std::cout << "\tcvt.s.w $f4,$f4" << std::endl;
            std::cout << "\t c.eq.s $f0,$f4" << std::endl;
            std::cout << "\t bc1t " << nextlabel << std::endl;
            std::cout << "\t nop" << std::endl;
        }
        else if (conditionType == "double")
        {
            std::cout << "\tadd $2,$0,$0" << std::endl;
            std::cout << "\tmtc1 $2,$f4" << std::endl;
            std::cout << "\tcvt.d.w $f4,$f4" << std::endl;
            std::cout << "\t c.eq.d $f0,$f4" << std::endl;
            std::cout << "\t bc1t " << nextlabel << std::endl;
            std::cout << "\t nop" << std::endl;
        }
        else
        {
            std::cout << "\t"
                      << "beq $2,$0," << nextlabel << std::endl;
            std::cout << "\t"
                      << "nop" << std::endl;
        }
        _body->compile(new_context);
        std::cout << continuelabel << ": " << std::endl;
        if (_exp3 != nullptr)
        {
            _exp3->compile(new_context);
        }
        std::cout << "\t"
                  << "b " << forlabel << std::endl;
        std::cout << "\t"
                  << "nop" << std::endl;
        std::cout << nextlabel << ":" << std::endl;
        return "";
    }
};

class SwitchStatement : public Statement
{
private:
    ExpressionPtr _exp;
    StatementPtr _body;

public:
    SwitchStatement(ExpressionPtr e, StatementPtr b) : _exp(e), _body(b){};
    void print() override{};
    void translate(int tabs, TranslatorContext &context) override{};
    std::string compile(Context &context) override
    {

        std::string nextlabel = "C" + std::to_string(unique_num++);
        std::string switchlabel = "S" + std::to_string(unique_num++);
        Context new_context(nextlabel, "", context);
        _exp->compile(context);
        context.push_stack();
        std::cout << "\t"
                  << "sw $2,0($sp)" << std::endl;
        std::cout << "\t"
                  << "b " << switchlabel << std::endl;
        std::cout << "nop" << std::endl;
        _body->compile(new_context);
        std::cout << "\t"
                  << "b " << nextlabel << std::endl;
        std::cout << "\t nop" << std::endl;
        std::cout << switchlabel << ":" << std::endl;
        for (auto el : new_context.caseflags)
        {

            std::string num = el.substr(nextlabel.length() + 2);
            if (num.length() >= 1)
            {
                int value = std::stoi(num);
                std::cout << "\t"
                          << "addi $t2,$0," << value << std::endl;
                std::cout << "\t"
                          << "lw $2,0($sp)" << std::endl;
                std::cout << "\t"
                          << "beq $t2,$2," << el << std::endl;
                std::cout << "\t"
                          << "nop" << std::endl;
            }
            else
            {
                std::cout << "\tb " << nextlabel << "CA" << std::endl;
                std::cout << "\t"
                          << "nop" << std::endl;
            }
        }
        std::cout << nextlabel + ":" << std::endl;
        context.pop_stack();

        return "";
    }
};
class CaseStatement : public Statement
{
private:
    ExpressionPtr _exp;
    StatementPtr _body;

public:
    CaseStatement(ExpressionPtr e, StatementPtr b) : _exp(e), _body(b){};
    CaseStatement(StatementPtr b) : _exp(nullptr), _body(b){};
    void print() override{};
    void translate(int tabs, TranslatorContext &context) override{};
    std::string compile(Context &context) override
    {
        std::string nextlabel = "C" + std::to_string(unique_num++);
        if (_exp == nullptr)
        {
            std::string caselabel = context.getBflag() + "CA";
            context.caseflags.push_back(caselabel);
            std::cout << caselabel << ":" << std::endl;
            _body->compile(context);
        }
        else
        {
            std::string caselabel = context.getBflag() + "CA" + std::to_string(std::dynamic_pointer_cast<Constant>(_exp)->getValue());
            context.caseflags.push_back(caselabel);
            std::cout << caselabel << ":" << std::endl;
            _body->compile(context);
            std::cout << nextlabel + ":" << std::endl;
        }
        return "";
    }
};
#endif