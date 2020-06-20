#ifndef AST_UNARY_H
#define AST_UNARY_H
#include <string>
#include "ast_expression.h"
class Unary
    : public Expression
{
protected:
    ExpressionPtr _expr;
    std::string _type;

public:
    Unary(const ExpressionPtr expr, std::string type)
        : _expr(expr), _type(type)
    {
    }

    ExpressionPtr getExpr() const
    {
        return _expr;
    }
    std::string getType() const
    {
        return _type;
    }
    virtual void print()
    {
        std::cout << "( " << _type << " ";
        _expr->print();
        std::cout << " )";
    }

    virtual void translate(int tabs, TranslatorContext &context)
    {
        std::cout << _type << "(";
        _expr->translate(tabs, context);
        std::cout << ")";
    }

    virtual std::string compile(Context &context)
    {
        std::string unaryType = _expr->compile(context);
        if (_type == "+")
            return "";
        else if (_type == "-")
        {
            if (unaryType == "float")
            {
                std::cout << "mtc1 $0,$f6" << std::endl;
                std::cout << "cvt.s.w $f6,$f6" << std::endl;
                std::cout << "\t sub.s $f0,$f6,$f0" << std::endl;
            }
            else if (unaryType == "double")
            {
                std::cout << "mtc1 $0,$f6" << std::endl;
                std::cout << "mtc1 $0,$f7" << std::endl;
                std::cout << "cvt.d.w $f6,$f6" << std::endl;
                std::cout << "\t sub.d $f0,$f6,$f0" << std::endl;
            }
            else
            {
                std::cout << "\t"
                          << "sub $2,$0,$2" << std::endl;
            }
        }
        else if (_type == "!")
        {
            std::cout << "\t"
                      << "sltu $2,$0,$2" << std::endl;
            std::cout << "\t xori $2,$2,1" << std::endl;
        }
        else if (_type == "~")
        {
            std::cout << "\t"
                      << "nor $2,$0,$2" << std::endl;
        }
        else if (_type == "&")
        {
            std::static_pointer_cast<Identifiable>(_expr)->getAddress(context);
        }
        else if (_type == "*")
        {
        }
        return unaryType;
    }
};

class PrefixOperator : public Unary
{
public:
    PrefixOperator(const ExpressionPtr expr, std::string type) : Unary(expr, type) {}
    void print()
    {
        std::cout << "( " << this->getType() << " ";
        getExpr()->print();
        std::cout << " )";
    }
    void translate(int tabs, TranslatorContext &context)
    {
        std::cout << this->getType() << " ";
        getExpr()->translate(tabs, context);
    }
    bool isPointer(ExpressionPtr e, Context &context) const
    {
        if (std::dynamic_pointer_cast<Declaration>(e) != nullptr)
        {
            std::string id = std::static_pointer_cast<Declaration>(e)->getId();
            if (context.symbol_table[id]->getRecordType() == "pointer" || context.symbol_table[id]->getType().find("*") != std::string::npos)
                return true;
        }
        return false;
    };
    std::string compile(Context &context)
    {
        std::string variable_id = std::dynamic_pointer_cast<Variable>(_expr)->getId();
        int offset = std::static_pointer_cast<VariableRecord>(context.symbol_table[variable_id])->getOffset();
        std::string unaryType = getExpr()->compile(context);
        if (getType() == "++")
        {
            if (isPointer(_expr, context))
                std::cout << "\t"
                          << "addi $2,$2,4" << std::endl;
            else
            {
                if (unaryType == "float")
                {
                    std::cout << "addi $2,$0,1" << std::endl;
                    std::cout << "mtc1 $2,$f6" << std::endl;
                    std::cout << "cvt.s.w $f6,$f6" << std::endl;
                    std::cout << "add.s $f0,$f0,$f6" << std::endl;
                    std::cout << "s.s $f0," << offset << "($fp)" << std::endl;
                }
                else if (unaryType == "double")
                {
                    std::cout << "addi $2,$0,1" << std::endl;
                    std::cout << "mtc1 $2,$f6" << std::endl;
                    std::cout << "mtc1 $0,$f7" << std::endl;
                    std::cout << "cvt.d.w $f6,$f6" << std::endl;
                    std::cout << "add.d $f0,$f0,$f6" << std::endl;
                    std::cout << "s.s $f0," << offset << "($fp)" << std::endl;
                    std::cout << "swc1 $f1," << offset + 4 << "($fp)" << std::endl;
                }
                else
                {
                    std::cout << "\t"
                              << "addi $2,$2,1" << std::endl;
                    std::cout << "\t"
                              << "sw $2," << offset << "($fp)" << std::endl;
                }
            }
        }
        else if (getType() == "--")
        {

            if (isPointer(_expr, context))
                std::cout << "\t"
                          << "addi $2,$2,-4" << std::endl;
            else
            {
                if (unaryType == "float")
                {
                    std::cout << "addi $2,$0,-1" << std::endl;
                    std::cout << "mtc1 $2,$f6" << std::endl;
                    std::cout << "cvt.s.w $f6,$f6" << std::endl;
                    std::cout << "add.s $f0,$f0,$f6" << std::endl;
                    std::cout << "s.s $f0," << offset << "($fp)" << std::endl;
                }
                else if (unaryType == "double")
                {
                    std::cout << "addi $2,$0,-1" << std::endl;
                    std::cout << "mtc1 $2,$f6" << std::endl;
                    std::cout << "mtc1 $0,$f7" << std::endl;
                    std::cout << "cvt.d.w $f6,$f6" << std::endl;
                    std::cout << "add.d $f0,$f0,$f6" << std::endl;
                    std::cout << "s.s $f0," << offset << "($fp)" << std::endl;
                    std::cout << "swc1 $f1," << offset + 4 << "($fp)" << std::endl;
                }
                else
                {
                    std::cout << "\t"
                              << "addi $2,$2,-1" << std::endl;
                    std::cout << "\t"
                              << "sw $2," << offset << "($fp)" << std::endl;
                }
            }
        }
        else
        {
            std::cerr << "weird prefix type: " << getType() << std::endl;
        }

        return unaryType;
    }
}

;

class PostfixOperator : public Unary
{
public:
    PostfixOperator(const ExpressionPtr expr, std::string type) : Unary(expr, type) {}
    virtual void print()
    {
        std::cout << "( "
                  << " ";
        getExpr()->print();
        std::cout << this->getType() << " )";
    }
    virtual void translate(int tabs, TranslatorContext &context)
    {
        getExpr()->translate(tabs, context);
        std::cout << this->getType();
    }
    bool isPointer(ExpressionPtr e, Context &context) const
    {
        if (std::dynamic_pointer_cast<Declaration>(e) != nullptr)
        {
            std::string id = std::static_pointer_cast<Declaration>(e)->getId();
            if (context.symbol_table[id]->getRecordType() == "pointer" || context.symbol_table[id]->getType().find("*") != std::string::npos)
                return true;
        }
        return false;
    };
    std::string compile(Context &context)
    {
        std::string variable_id = std::dynamic_pointer_cast<Variable>(_expr)->getId();
        int offset = std::static_pointer_cast<VariableRecord>(context.symbol_table[variable_id])->getOffset();
        std::string unaryType = getExpr()->compile(context);
        if (getType() == "++")
        {
            if (isPointer(_expr, context))
                std::cout << "\t"
                          << "addi $t0,$2,4" << std::endl;
            else
            {
                if (unaryType == "float")
                {
                    std::cout << "addi $2,$0,1" << std::endl;
                    std::cout << "mtc1 $2,$f6" << std::endl;
                    std::cout << "cvt.s.w $f6,$f6" << std::endl;
                    std::cout << "add.s $f6,$f0,$f6" << std::endl;
                    std::cout << "s.s $f6," << offset << "($fp)" << std::endl;
                }
                else if (unaryType == "double")
                {
                    std::cout << "addi $2,$0,1" << std::endl;
                    std::cout << "mtc1 $2,$f6" << std::endl;
                    std::cout << "mtc1 $0,$f7" << std::endl;
                    std::cout << "cvt.d.w $f6,$f6" << std::endl;
                    std::cout << "add.d $f6,$f0,$f6" << std::endl;
                    std::cout << "sw.d $f6," << offset << "($fp)" << std::endl;
                    // std::cout << "swc1 $f7," << offset + 4 << "($fp)" << std::endl;
                }
                else
                {
                    std::cout << "\t"
                              << "addi $t0,$2,1" << std::endl;
                    std::cout << "\t"
                              << "sw $t0," << offset << "($fp)" << std::endl;
                }
            }
        }
        else if (getType() == "--")
        {

            if (isPointer(_expr, context))
                std::cout << "\t"
                          << "addi $t0,$2,-4" << std::endl;
            else
            {
                if (unaryType == "float")
                {
                    std::cout << "addi $2,$0,-1" << std::endl;
                    std::cout << "mtc1 $2,$f6" << std::endl;
                    std::cout << "cvt.s.w $f6,$f6" << std::endl;
                    std::cout << "add.s $f6,$f0,$f6" << std::endl;
                    std::cout << "s.s $f6," << offset << "($fp)" << std::endl;
                }
                else if (unaryType == "double")
                {
                    std::cout << "addi $2,$0,-1" << std::endl;
                    std::cout << "mtc1 $2,$f6" << std::endl;
                    std::cout << "mtc1 $0,$f7" << std::endl;
                    std::cout << "cvt.d.w $f6,$f6" << std::endl;
                    std::cout << "add.d $f6,$f0,$f6" << std::endl;
                    std::cout << "s.s $f6," << offset << "($fp)" << std::endl;
                    std::cout << "s.s $f7," << offset + 4 << "($fp)" << std::endl;
                }
                else
                {
                    std::cout << "\t"
                              << "addi $t0,$2,-1" << std::endl;
                    std::cout << "\t"
                              << "sw $t0," << offset << "($fp)" << std::endl;
                }
            }
        }
        else
        {
            std::cerr << "weird postfix type: " << getType() << std::endl;
        }
        return unaryType;
    }
};

class Dereference : public Identifiable, public Assignable
{
private:
    ExpressionPtr location;

public:
    Dereference(ExpressionPtr p) : location(p){};
    std::string compile(Context &context) override
    {
        std::string unaryType = location->compile(context);
        if (unaryType == "float")
        {
            std::cout << "\t mfc1 $t0,$f0" << std::endl;
            std::cout << "\t l.s $f0,0($t0)" << std::endl;
            std::cout << "nop" << std::endl;
        }
        else if (unaryType == "double")
        {
            std::cout << "\t mfc1 $t0,$f0" << std::endl;
            std::cout << "\t l.s $f0,0($t0)" << std::endl;
            std::cout << "nop" << std::endl;
            std::cout << "\t l.s $f1,4($t0)" << std::endl;
            std::cout << "nop" << std::endl;
        }
        else
        {
            std::cout << "\t"
                      << "lw $2,0($2)" << std::endl;
            std::cout << "nop" << std::endl;
        }

        return unaryType;
    }
    void assign(Context &context, std::string &type) override
    {
        context.push_stack();
        std::cout << "\t"
                  << "sw $2,0($sp)" << std::endl;
        location->compile(context);
        std::cout << "\t"
                  << "lw $t1,0($sp)" << std::endl;
        context.pop_stack();
        std::cout << "\t"
                  << "sw $t1,0($2)" << std::endl;
        std::cout << "\t"
                  << "add $2,$t1,$0" << std::endl;
    }
    std::string getId() override
    {
        return std::static_pointer_cast<Identifiable>(location)->getId();
    }
    void getAddress(Context &context) const override
    {
        location->compile(context);
    };
};

class SizeOf : public Expression
{
private:
    std::string _value;

public:
    SizeOf(std::string &val) : _value(val){};
    std::string compile(Context &context) override
    {
        std::string type;
        if ((_value == "double") || (_value == "int") || (_value == "float") || (_value == "char") || (_value == "unsigned"))
            type = _value;
        else
            type = context.symbol_table[_value]->getType();
        if (type == "char")
            std::cout
                << "\t addi $2,$0,1" << std::endl;
        else if (type == "double")
            std::cout
                << "\t addi $2,$0,8" << std::endl;
        else
        {
            std::cout << "\t addi $2,$0,4" << std::endl;
        }

        return "int";
    }
};
#endif