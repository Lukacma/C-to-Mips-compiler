#ifndef AST_OPERATORS_H
#define AST_OPERATORS_H

#include <string>
#include "ast_expression.h"
#include "ast_primitives.h"
extern int unique_num;
class BinaryOperator
    : public Expression
{
private:
    ExpressionPtr _left;
    ExpressionPtr _right;
    std::string _type;

public:
    BinaryOperator(ExpressionPtr left, ExpressionPtr right, std::string type)
        : _left(left), _right(right), _type(type)
    {
    }

    ExpressionPtr getLeft() const
    {
        return _left;
    }

    ExpressionPtr getRight() const
    {
        return _right;
    }
    std::string getType()
    {
        return _type;
    }
    void print()
    {
        std::cout << "( ";
        _left->print();
        std::cout << " " << _type << " ";
        _right->print();
        std::cout << " )";
    }
    void translate(int tabs, TranslatorContext &context)
    {
        //std::cout << std::string(tabs, '\t');
        if (_type != "=")
            std::cout << "(";
        _left->translate(tabs, context);
        if (_type != "=")
            std::cout << ")";
        if ((_type != "&&") && (_type != "||"))
        {
            std::cout << " " << _type << " ";
        }
        else
        {
            if (_type == "&&")
                std::cout << " "
                          << "and"
                          << " ";
            if (_type == "||")
                std::cout << " "
                          << "or"
                          << " ";
        }
        std::cout << "(";
        _right->translate(tabs, context);
        std::cout << ")";
    }

    bool isPointer(ExpressionPtr e, Context &context) const
    {
        if (std::dynamic_pointer_cast<Declaration>(e) != nullptr)
        {
            std::string id = std::static_pointer_cast<Declaration>(e)->getId();
            if ((context.symbol_table[id]->getRecordType() == "pointer") || (context.symbol_table[id]->getType().find("*") != std::string::npos))
                return true;
        }
        return false;
    };
    void doPointerArith(Context &context) const
    {
        if (isPointer(_left, context))
        {
            std::cout << "\t"
                      << "addi $t3,$0,4" << std::endl;
            std::cout << "\t"
                      << "mul $2,$2,$t3" << std::endl;
        }
        else if (isPointer(_right, context))
        {
            std::cout << "\t"
                      << "addi $t3,$0,4" << std::endl;
            std::cout << "\t"
                      << "mul $t0,$t0,$t3" << std::endl;
        }
    };
    std::string compile(Context &context)
    {
        std::string leftType = _left->compile(context);
        if (leftType == "float")
        {
            context.push_stack();
            std::cout << "\t"
                      << "s.s $f0,0($sp)" << std::endl;
            std::cout << "\t nop" << std::endl;
        }
        else if (leftType == "double")
        {
            context.push_stack();
            std::cout << "\t"
                      << "s.s $f1,0($sp)" << std::endl;
            std::cout << "\t nop" << std::endl;
            context.push_stack();
            std::cout << "\t"
                      << "s.s $f0,0($sp)" << std::endl;
            std::cout << "\t nop" << std::endl;
        }
        else
        {
            context.push_stack();
            std::cout << "\t"
                      << "sw $2,0($sp)" << std::endl;
            std::cout << "\t nop" << std::endl;
        }
        std::string rightType = "int";
        if ((_type != "||") && (_type != "&&"))
        {
            rightType = _right->compile(context);
        }

        std::string chosenOne = chooseStrongest(leftType, rightType);

        if (leftType == "float")
        {
            std::cout << "\t"
                      << "l.s $f4,0($sp)" << std::endl;
            std::cout << "\t nop" << std::endl;
            context.pop_stack();
        }
        else if (leftType == "double")
        {

            std::cout << "\t"
                      << "l.s $f4,0($sp)" << std::endl;
            std::cout << "\t nop" << std::endl;
            context.pop_stack();
            std::cout << "\t"
                      << "l.s $f5,0($sp)" << std::endl;
            std::cout << "\t nop" << std::endl;
            context.pop_stack();
        }
        else
        {
            std::cout
                << "\t"
                << "lw $t0,0($sp)" << std::endl;
            context.pop_stack();
        }

        if (_type == "+")
        {
            doPointerArith(context);
            if (leftType == "float")
            {
                std::cout << "\t"
                          << "add.s $f0,$f0,$f4" << std::endl;
                return "float";
            }
            else if (leftType == "double")
            {
                std::cout << "\t"
                          << "add.d $f0,$f0,$f4" << std::endl;
                return "double";
            }
            else
            {
                std::cout << "\t"
                          << "addu $2,$2,$t0" << std::endl;
                return chosenOne;
            }
        }
        else if (_type == "-")
        {
            doPointerArith(context);
            if (leftType == "float")
            {
                std::cout << "\t"
                          << "sub.s $f0,$f0,$f4" << std::endl;
                return "float";
            }
            else if (leftType == "double")
            {
                std::cout << "\t"
                          << "sub.d $f0,$f0,$f4" << std::endl;
                return "double";
            }
            else
            {
                std::cout
                    << "\t"
                    << "subu $2,$t0,$2" << std::endl;

                return chosenOne;
            }
        }
        else if (_type == "*")
        {
            if (leftType == "float")
            {
                std::cout << "\t mul.s $f0,$f0,$f4" << std::endl;
                return "float";
            }
            else if (leftType == "double")
            {
                std::cout << "\t mul.d $f0,$f0,$f4" << std::endl;
                return "double";
            }
            else
            {
                std::cout
                    << "\t"
                    << "mul $2,$t0,$2 " << std::endl;

                return chosenOne;
            }
        }
        else if (_type == "/")
        {
            if (leftType == "float")
            {
                std::cout << "\t div.s $f0,$f4,$f0" << std::endl;
                return "float";
            }
            else if (leftType == "double")
            {
                std::cout << "\t div.d $f0,$f4,$f0" << std::endl;
                return "double";
            }
            else
            {
                std::cout << "\t"
                          << "div $t0,$2" << std::endl;
                std::cout << "\t"
                          << "mflo $2" << std::endl;

                return chosenOne;
            }
        }
        else if (_type == "%")
        {
            std::cout << "\t"
                      << "div $t0,$2" << std::endl;
            std::cout << "\t"
                      << "mfhi $2" << std::endl;

            return chosenOne;
        }
        else if (_type == "&")
        {
            std::cout
                << "\t"
                << "and $2,$2,$t0" << std::endl;

            return chosenOne;
        }
        else if (_type == "|")
        {
            std::cout
                << "\t"
                << "or $2,$2,$t0" << std::endl;

            return chosenOne;
        }
        else if (_type == "^")
        {
            std::cout
                << "\t"
                << "xor $2,$2,$t0" << std::endl;

            return chosenOne;
        }
        else if (_type == "<<")
        {
            std::cout
                << "\t"
                << "sll $2,$t0,$2" << std::endl;

            return leftType;
        }
        else if (_type == ">>")
        {
            if (leftType == "unsigned")
            {
                std::cout
                    << "\t"
                    << "srl $2,$t0,$2" << std::endl;
            }
            else
            {
                std::cout
                    << "\t"
                    << "sra $2,$t0,$2" << std::endl;
            }
            return leftType;
        }
        else if (_type == "==")
        {
            std::string FalseFlag = "FF" + std::to_string(unique_num++);
            std::string ContinueFlag = "CF" + std::to_string(unique_num++);
            if (leftType == "float")
            {
                std::cout << "c.eq.s  $f4,$f0" << std::endl;
                std::cout << "bc1f " << FalseFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "addi $2,$0,1" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "cvt.s.w $f0,$f0" << std::endl;
                std::cout << "b " << ContinueFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << FalseFlag << ": " << std::endl;
                std::cout << "addi $2,$0,0" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "cvt.s.w $f0,$f0" << std::endl;
                std::cout << ContinueFlag << ":" << std::endl;
                return "float";
            }
            else if (leftType == "double")
            {
                std::cout << "c.eq.d  $f4,$f0" << std::endl;
                std::cout << "bc1f " << FalseFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "addi $2,$0,1" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "mtc1 $0,$f1" << std::endl;
                std::cout << "cvt.d.w $f0,$f0" << std::endl;
                std::cout << "b " << ContinueFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << FalseFlag << ": " << std::endl;
                std::cout << "addi $2,$0,0" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "mtc1 $0,$f1" << std::endl;
                std::cout << "cvt.d.w $f0,$f0" << std::endl;
                std::cout << ContinueFlag << ":" << std::endl;
                return "double";
            }
            else
            {
                std::cout << "\t"
                          << "xor $2,$t0,$2" << std::endl;
                std::cout << "\t"
                          << "sltu $2,$0,$2" << std::endl;
                std::cout << "\t xori $2,$2,1" << std::endl;
                return "int";
            }
        }
        else if (_type == "!=")
        {
            std::string FalseFlag = "FF" + std::to_string(unique_num++);
            std::string ContinueFlag = "CF" + std::to_string(unique_num++);
            if (leftType == "float")
            {
                std::cout << "c.eq.s  $f4,$f0" << std::endl;
                std::cout << "bc1t " << FalseFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "addi $2,$0,1" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "cvt.s.w $f0,$f0" << std::endl;
                std::cout << "b " << ContinueFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << FalseFlag << ": " << std::endl;
                std::cout << "addi $2,$0,0" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "cvt.s.w $f0,$f0" << std::endl;
                std::cout << ContinueFlag << ":" << std::endl;
                return "float";
            }
            else if (leftType == "double")
            {
                std::cout << "c.eq.d  $f4,$f0" << std::endl;
                std::cout << "bc1t " << FalseFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "addi $2,$0,1" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "mtc1 $0,$f1" << std::endl;
                std::cout << "cvt.d.w $f0,$f0" << std::endl;
                std::cout << "b " << ContinueFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << FalseFlag << ": " << std::endl;
                std::cout << "addi $2,$0,0" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "mtc1 $0,$f1" << std::endl;
                std::cout << "cvt.d.w $f0,$f0" << std::endl;
                std::cout << ContinueFlag << ":" << std::endl;
                return "double";
            }
            else
            {
                std::cout << "\t"
                          << "xor $2,$t0,$2" << std::endl;
                std::cout << "\t"
                          << "sltu $2,$0,$2" << std::endl;
                return "int";
            }
        }
        else if (_type == ">=")
        {
            std::string FalseFlag = "FF" + std::to_string(unique_num++);
            std::string ContinueFlag = "CF" + std::to_string(unique_num++);
            if (leftType == "float")
            {
                std::cout << "c.lt.s  $f4,$f0" << std::endl;
                std::cout << "bc1t " << FalseFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "addi $2,$0,1" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "cvt.s.w $f0,$f0" << std::endl;
                std::cout << "b " << ContinueFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << FalseFlag << ": " << std::endl;
                std::cout << "addi $2,$0,0" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "cvt.s.w $f0,$f0" << std::endl;
                std::cout << ContinueFlag << ":" << std::endl;
                return "float";
            }
            else if (leftType == "double")
            {
                std::cout << "c.lt.d  $f4,$f0" << std::endl;
                std::cout << "bc1t " << FalseFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "addi $2,$0,1" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "mtc1 $0,$f1" << std::endl;
                std::cout << "cvt.d.w $f0,$f0" << std::endl;
                std::cout << "b " << ContinueFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << FalseFlag << ": " << std::endl;
                std::cout << "addi $2,$0,0" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "mtc1 $0,$f1" << std::endl;
                std::cout << "cvt.d.w $f0,$f0" << std::endl;
                std::cout << ContinueFlag << ":" << std::endl;
                return "double";
            }
            else
            {
                if (chosenOne == "unsigned")
                {
                    std::cout << "\t"
                              << "sltu $2,$t0,$2" << std::endl;
                    std::cout << "\t"
                              << "xori $2,$2,0x1" << std::endl;
                }
                else
                {
                    std::cout << "\t"
                              << "slt $2,$t0,$2" << std::endl;
                    std::cout << "\t"
                              << "xori $2,$2,0x1" << std::endl;
                }
                return "int";
            }
        }
        else if (_type == "<=")
        {
            std::string FalseFlag = "FF" + std::to_string(unique_num++);
            std::string ContinueFlag = "CF" + std::to_string(unique_num++);
            if (leftType == "float")
            {
                std::cout << "c.le.s  $f4,$f0" << std::endl;
                std::cout << "bc1f " << FalseFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "addi $2,$0,1" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "cvt.s.w $f0,$f0" << std::endl;
                std::cout << "b " << ContinueFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << FalseFlag << ": " << std::endl;
                std::cout << "addi $2,$0,0" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "cvt.s.w $f0,$f0" << std::endl;
                std::cout << ContinueFlag << ":" << std::endl;
                return "float";
            }
            else if (leftType == "double")
            {
                std::cout << "c.le.d  $f4,$f0" << std::endl;
                std::cout << "bc1f " << FalseFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "addi $2,$0,1" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "mtc1 $0,$f1" << std::endl;
                std::cout << "cvt.d.w $f0,$f0" << std::endl;
                std::cout << "b " << ContinueFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << FalseFlag << ": " << std::endl;
                std::cout << "addi $2,$0,0" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "mtc1 $0,$f1" << std::endl;
                std::cout << "cvt.d.w $f0,$f0" << std::endl;
                std::cout << ContinueFlag << ":" << std::endl;
                return "double";
            }
            else
            {
                if (chosenOne == "unsigned")
                {
                    std::cout << "\t"
                              << "sltu $2,$2,$t0" << std::endl;
                    std::cout << "\t"
                              << "xori $2,$2,0x1" << std::endl;
                }
                else
                {
                    std::cout << "\t"
                              << "slt $2,$2,$t0" << std::endl;
                    std::cout << "\t"
                              << "xori $2,$2,0x1" << std::endl;
                }
                return "int";
            }
        }
        else if (_type == "<")
        {
            std::string FalseFlag = "FF" + std::to_string(unique_num++);
            std::string ContinueFlag = "CF" + std::to_string(unique_num++);
            if (leftType == "float")
            {
                std::cout << "c.lt.s  $f4,$f0" << std::endl;
                std::cout << "bc1f " << FalseFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "addi $2,$0,1" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "cvt.s.w $f0,$f0" << std::endl;
                std::cout << "b " << ContinueFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << FalseFlag << ": " << std::endl;
                std::cout << "addi $2,$0,0" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "cvt.s.w $f0,$f0" << std::endl;
                std::cout << ContinueFlag << ":" << std::endl;
                return "float";
            }
            else if (leftType == "double")
            {
                std::cout << "c.lt.d  $f4,$f0" << std::endl;
                std::cout << "bc1f " << FalseFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "addi $2,$0,1" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "mtc1 $0,$f1" << std::endl;
                std::cout << "cvt.d.w $f0,$f0" << std::endl;
                std::cout << "b " << ContinueFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << FalseFlag << ": " << std::endl;
                std::cout << "addi $2,$0,0" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "mtc1 $0,$f1" << std::endl;
                std::cout << "cvt.d.w $f0,$f0" << std::endl;
                std::cout << ContinueFlag << ":" << std::endl;
                return "double";
            }
            else
            {
                if (chosenOne == "unsigned")
                {
                    std::cout << "\t"
                              << "sltu $2,$t0,$2" << std::endl;
                }
                else
                {
                    std::cout << "\t"
                              << "slt $2,$t0,$2" << std::endl;
                }
                return "int";
            }
        }
        else if (_type == ">")
        {
            std::string FalseFlag = "FF" + std::to_string(unique_num++);
            std::string ContinueFlag = "CF" + std::to_string(unique_num++);
            if (leftType == "float")
            {
                std::cout << "c.le.s  $f4,$f0" << std::endl;
                std::cout << "bc1t " << FalseFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "addi $2,$0,1" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "cvt.s.w $f0,$f0" << std::endl;
                std::cout << "b " << ContinueFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << FalseFlag << ": " << std::endl;
                std::cout << "addi $2,$0,0" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "cvt.s.w $f0,$f0" << std::endl;
                std::cout << ContinueFlag << ":" << std::endl;
                return "float";
            }
            else if (leftType == "double")
            {
                std::cout << "c.le.d  $f4,$f0" << std::endl;
                std::cout << "bc1t " << FalseFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << "addi $2,$0,1" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "mtc1 $0,$f1" << std::endl;
                std::cout << "cvt.d.w $f0,$f0" << std::endl;
                std::cout << "b " << ContinueFlag << std::endl;
                std::cout << "nop" << std::endl;
                std::cout << FalseFlag << ": " << std::endl;
                std::cout << "addi $2,$0,0" << std::endl;
                std::cout << "mtc1 $2,$f0" << std::endl;
                std::cout << "mtc1 $0,$f1" << std::endl;
                std::cout << "cvt.d.w $f0,$f0" << std::endl;
                std::cout << ContinueFlag << ":" << std::endl;
                return "double";
            }
            else
            {
                if (chosenOne == "unsigned")
                {
                    std::cout << "\t"
                              << "sltu $2,$2,$t0" << std::endl;
                }
                else
                {
                    std::cout << "\t"
                              << "slt $2,$2,$t0" << std::endl;
                }
                return "int";
            }
        }
        else if (_type == "&&")
        {
            std::string falselabel = "F" + std::to_string(unique_num++);
            std::string truelabel = "T" + std::to_string(unique_num++);
            if (leftType == "float")
            {
                std::cout << "\tadd $2,$0,$0" << std::endl;
                std::cout << "\tmtc1 $2,$f6" << std::endl;
                std::cout << "\tcvt.s.w $f6,$f6" << std::endl;
                std::cout << "\t c.eq.s $f4,$f6" << std::endl;
                std::cout << "\t bc1t " << falselabel << std::endl;
            }
            else if (leftType == "double")
            {
                std::cout << "\tadd $2,$0,$0" << std::endl;
                std::cout << "\tmtc1 $2,$f6" << std::endl;
                std::cout << "\tcvt.d.w $f6,$f6" << std::endl;
                std::cout << "\t c.eq.d $f4,$f6" << std::endl;
                std::cout << "\t bc1t " << falselabel << std::endl;
            }
            else
            {
                std::cout << "\t"
                          << "beq $t0,$0," << falselabel << std::endl;
                std::cout << "\t"
                          << "nop" << std::endl;
            }
            _right->compile(context);
            if (leftType == "float")
            {
                std::cout << "\tadd $2,$0,$0" << std::endl;
                std::cout << "\tmtc1 $2,$f6" << std::endl;
                std::cout << "\tcvt.s.w $f6,$f6" << std::endl;
                std::cout << "\t c.eq.s $f0,$f6" << std::endl;
                std::cout << "\t bc1t " << falselabel << std::endl;
            }
            else if (leftType == "double")
            {
                std::cout << "\tadd $2,$0,$0" << std::endl;
                std::cout << "\tmtc1 $2,$f6" << std::endl;
                std::cout << "\tcvt.d.w $f6,$f6" << std::endl;
                std::cout << "\t c.eq.d $f0,$f6" << std::endl;
                std::cout << "\t bc1t " << falselabel << std::endl;
            }
            else
            {
                std::cout << "\t"
                          << "beq $2,$0," << falselabel << std::endl;
                std::cout << "\t"
                          << "nop" << std::endl;
            }
            std::cout << "\t"
                      << "li $2,1" << std::endl;
            std::cout << "\t"
                      << "b " << truelabel << std::endl;
            std::cout << "\t"
                      << "nop" << std::endl;
            std::cout << falselabel << ":" << std::endl;
            std::cout << "\t"
                      << "move $2,$0" << std::endl;
            std::cout << truelabel << ":" << std::endl;

            return "int";
        }
        else if (_type == "||")
        {
            std::string falselabel = "F" + std::to_string(unique_num++);
            std::string truelabel = "T" + std::to_string(unique_num++);
            std::string label = "L" + std::to_string(unique_num++);
            if (leftType == "float")
            {
                std::cout << "\tadd $2,$0,$0" << std::endl;
                std::cout << "\tmtc1 $2,$f6" << std::endl;
                std::cout << "\tcvt.s.w $f6,$f6" << std::endl;
                std::cout << "\t c.eq.s $f4,$f6" << std::endl;
                std::cout << "\t bc1f " << truelabel << std::endl;
            }
            else if (leftType == "double")
            {
                std::cout << "\tadd $2,$0,$0" << std::endl;
                std::cout << "\tmtc1 $2,$f6" << std::endl;
                std::cout << "\tcvt.d.w $f6,$f6" << std::endl;
                std::cout << "\t c.eq.d $f4,$f6" << std::endl;
                std::cout << "\t bc1f " << truelabel << std::endl;
            }
            else
            {
                std::cout << "\t"
                          << "bne $t0,$0," << truelabel << std::endl;
                std::cout << "\t"
                          << "nop" << std::endl;
            }
            _right->compile(context);
            if (leftType == "float")
            {
                std::cout << "\tadd $2,$0,$0" << std::endl;
                std::cout << "\tmtc1 $2,$f6" << std::endl;
                std::cout << "\tcvt.s.w $f6,$f6" << std::endl;
                std::cout << "\t c.eq.s $f0,$f6" << std::endl;
                std::cout << "\t bc1t " << falselabel << std::endl;
            }
            else if (leftType == "double")
            {
                std::cout << "\tadd $2,$0,$0" << std::endl;
                std::cout << "\tmtc1 $2,$f6" << std::endl;
                std::cout << "\tcvt.d.w $f6,$f6" << std::endl;
                std::cout << "\t c.eq.d $f0,$f6" << std::endl;
                std::cout << "\t bc1t " << falselabel << std::endl;
            }
            else
            {
                std::cout << "\t"
                          << "beq $2,$0," << falselabel << std::endl;
                std::cout << "\t"
                          << "nop" << std::endl;
            }
            std::cout << truelabel << ":" << std::endl;
            std::cout << "\t"
                      << "li $2,1" << std::endl;
            std::cout << "\t"
                      << "b " << label << std::endl;
            std::cout << "\t"
                      << "nop" << std::endl;
            std::cout << falselabel << ":" << std::endl;
            std::cout << "\t"
                      << "move $2,$0" << std::endl;
            std::cout << label << ":" << std::endl;

            return "int";
        }
        else if (_type == "=")
        {
            std::dynamic_pointer_cast<Assignable>(_left)->assign(context, leftType);
            return leftType;
        }
        else if (_type == "+=")
        {
            doPointerArith(context);
            if (leftType == "float")
                std::cout << "\t add.s $f0,$f4,$f0" << std::endl;
            else if (leftType == "double")
                std::cout << "\t add.d $f0,$f4,$f0" << std::endl;
            else
                std::cout << "\t"
                          << "ADD $2,$2,$t0 " << std::endl;
            std::dynamic_pointer_cast<Assignable>(_left)->assign(context, leftType);
            return leftType;
        }
        else if (_type == "-=")
        {
            doPointerArith(context);
            if (leftType == "float")
                std::cout << "\t sub.s $f0,$f4,$f0" << std::endl;
            else if (leftType == "double")
                std::cout << "\t sub.d $f0,$f4,$f0" << std::endl;
            else
                std::cout << "\t"
                          << "Sub $2,$t0 ,$2" << std::endl;
            std::dynamic_pointer_cast<Assignable>(_left)->assign(context, leftType);
            return leftType;
        }
        else if (_type == "*=")
        {
            if (leftType == "float")
                std::cout << "\t mul.s $f0,$f4,$f0" << std::endl;
            else if (leftType == "double")
                std::cout << "\t mul.d $f0,$f4,$f0" << std::endl;
            else
                std::cout << "\t"
                          << "mul $2,$t0 ,$2" << std::endl;

            std::dynamic_pointer_cast<Assignable>(_left)->assign(context, leftType);

            return leftType;
        }

        else if (_type == "/=")
        {
            if (leftType == "float")
                std::cout << "\t div.s $f0,$f4,$f0" << std::endl;
            else if (leftType == "double")
                std::cout << "\t div.d $f0,$f4,$f0" << std::endl;
            else
            {
                std::cout << "\t"
                          << "div $t0,$2" << std::endl;
                std::cout << "\t"
                          << "mflo $2" << std::endl;
            }
            std::dynamic_pointer_cast<Assignable>(_left)->assign(context, leftType);

            return leftType;
        }
        else if (_type == "%=")
        {
            std::cout << "\t"
                      << "div $t0,$2" << std::endl;
            std::cout << "\t"
                      << "mfhi $2" << std::endl;
            std::dynamic_pointer_cast<Assignable>(_left)->assign(context, leftType);

            return leftType;
        }
        else if (_type == "<<=")
        {
            std::cout << "\t"
                      << "sll $2,$t0,$2" << std::endl;
            std::dynamic_pointer_cast<Assignable>(_left)->assign(context, leftType);
            return leftType;
        }
        else if (_type == ">>=")
        {
            if (leftType == "unsigned")
            {
                std::cout << "\t"
                          << "srl $2,$t0,$2" << std::endl;
                std::dynamic_pointer_cast<Assignable>(_left)->assign(context, leftType);
            }
            else
            {
                std::cout << "\t"
                          << "sra $2,$t0,$2" << std::endl;
                std::dynamic_pointer_cast<Assignable>(_left)->assign(context, leftType);
            }
            return leftType;
        }
        else if (_type == "&=")
        {
            std::cout << "\t"
                      << "and $2,$t0,$2" << std::endl;
            std::dynamic_pointer_cast<Assignable>(_left)->assign(context, leftType);

            return leftType;
        }
        else if (_type == "|=")
        {

            std::cout << "\t"
                      << "or $2,$t0,$2" << std::endl;
            std::dynamic_pointer_cast<Assignable>(_left)->assign(context, leftType);

            return leftType;
        }
        else if (_type == "^=")
        {
            std::cout << "\t"
                      << "xor $2,$t0,$2" << std::endl;
            std::dynamic_pointer_cast<Assignable>(_left)->assign(context, leftType);

            return leftType;
        }
        else
        {
            std::cerr << "You forgot something you idiot !!!!" << std::endl;
            return "Oh no.";
        }
    }

    std::string chooseStrongest(std::string a, std::string b)
    {
        if ((a == "double") || (b == "double"))
            return "double";
        if ((a == "float") || (b == "float"))
            return "float";
        if ((a == "unsigned") || (b == "unsigned"))
            return "unsigned";
        return "int";
    }
};

class TernaryOperator
    : public Expression
{
private:
    ExpressionPtr _condition;
    ExpressionPtr _holds;
    ExpressionPtr _other;

public:
    TernaryOperator(ExpressionPtr condition, ExpressionPtr holds, ExpressionPtr other)
        : _condition(condition), _holds(holds), _other(other)
    {
    }

    ExpressionPtr getCondition() const
    {
        return _condition;
    }

    ExpressionPtr getTrue() const
    {
        return _holds;
    }
    ExpressionPtr getFalse()
    {
        return _other;
    }
    void print()
    {
        std::cout << "(( ";
        _condition->print();
        std::cout << ")? ";
        _holds->print();
        std::cout << ": ";
        _other->print();
        std::cout << " )";
    }

    void translate(int tabs, TranslatorContext &context)
    {
        std::cout << "(( ";
        _condition->translate(tabs, context);
        std::cout << ")? ";
        _holds->translate(tabs, context);
        std::cout << ": ";
        _other->translate(tabs, context);
        std::cout << " )";
    }

    std::string compile(Context &context)
    {
        std::string falselabel = "F" + std::to_string(unique_num++);
        std::string nextlabel = "N" + std::to_string(unique_num++);
        std::string conditionType = _condition->compile(context);
        if (conditionType == "float")
        {
            std::cout << "\tadd $2,$0,$0" << std::endl;
            std::cout << "\tmtc1 $2,$f4" << std::endl;
            std::cout << "\tcvt.s.w $f4,$f4" << std::endl;
            std::cout << "\t c.eq.s $f0,$f4" << std::endl;
            std::cout << "\t bc1t " << falselabel << std::endl;
        }
        else if (conditionType == "double")
        {
            std::cout << "\tadd $2,$0,$0" << std::endl;
            std::cout << "\tmtc1 $2,$f4" << std::endl;
            std::cout << "\tcvt.d.w $f4,$f4" << std::endl;
            std::cout << "\t c.eq.d $f0,$f4" << std::endl;
            std::cout << "\t bc1t " << falselabel << std::endl;
        }
        else
        {
            std::cout << "\t"
                      << "beq $2,$0," << falselabel << std::endl;
            std::cout << "\t"
                      << "nop" << std::endl;
        }
        std::string expType = _holds->compile(context);
        std::cout << "\t"
                  << "b " << nextlabel << std::endl;
        std::cout << "\t"
                  << "nop" << std::endl;
        std::cout << falselabel << ":" << std::endl;
        _other->compile(context);
        std::cout << nextlabel << ":" << std::endl;

        return expType;
    }
};

#endif // !AST_OPERATORS_H