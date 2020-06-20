#ifndef AST_PRIMITIVES_H
#define AST_PRIMITIVES_H
#include <string>
#include <numeric>
#include "ast_expression.h"
#include "ast_initializer.h"

class Constant : public Expression
{
private:
    long _value;

public:
    Constant(long value) : _value(value)
    {
    }
    int getValue()
    {
        return _value;
    }
    void print()
    {
        std::cout << " " << _value;
    }

    void translate(int tabs, TranslatorContext &context)
    {
        std::cout << " " << _value;
    }
    std::string compile(Context &context)
    {
        std::cout << "\t"
                  << "li $2," << _value << std::endl;
        return "int";
    }
};

class DConstant : public Expression
{
private:
    double _value;

public:
    DConstant(double value) : _value(value)
    {
    }
    double getValue()
    {

        return _value;
    }
    void print()
    {
        std::cout << " " << _value;
    }

    void translate(int tabs, TranslatorContext &context)
    {
        std::cout << " " << _value;
    }
    std::string compile(Context &context)
    {
        long int tmp = *reinterpret_cast<long int *>(&_value);
        long mask = 0XFFFFFFFF;
        int most_significant = tmp >> 32;
        int least_significant = tmp & mask;
        std::cout << "\t"
                  << "li $2," << most_significant << std::endl;
        std::cout << "\t mtc1 $2,$f1" << std::endl;
        std::cout << "\t"
                  << "li $2," << least_significant << std::endl;
        std::cout << "\t mtc1 $2,$f0" << std::endl;
        return "double";
    }
};

class FConstant : public Expression
{
private:
    float _value;

public:
    FConstant(float value) : _value(value)
    {
    }
    float getValue()
    {

        return _value;
    }
    void print()
    {
        std::cout << " " << _value;
    }

    void translate(int tabs, TranslatorContext &context)
    {
        std::cout << " " << _value;
    }
    std::string compile(Context &context)
    {
        int tmp = *reinterpret_cast<int *>(&_value);

        std::cout << "\t"
                  << "li $2," << tmp << std::endl;
        std::cout << "\t mtc1 $2,$f0" << std::endl;
        return "float";
    }
};

class Variable : public Declaration, public Assignable
{
private:
    ExpressionPtr _value;

public:
    Variable(std::string type, std::string id, ExpressionPtr value) : Declaration(id, "var", type), _value(value) {}

    Variable(std::string id) : Declaration(id, "var"), _value(nullptr) {}
    Variable(const Variable &old) : Declaration(old._id, old.type, "var"), _value(old.getValue()) {}

    ExpressionPtr getValue() const
    {
        return _value;
    }

    void setValue(std::shared_ptr<Constant> new_val)
    {
        _value = new_val;
    }

    void print()
    {
        std::cout << "[ ";
        if (type != "")
            std::cout << type << " ";
        if (_value != nullptr)
        {
            std::cout << this->type << " = ";
            _value->print();
        }
        else
            std::cout << this->type;
        if (type != "")
            std::cout << ";" << std::endl;
        std::cout << " ]";
    }

    void translate(int tabs, TranslatorContext &context)
    {
        if (tabs == 0)
            context.addGlobalVar(_id);

        if (_value != nullptr)
        {
            std::cout << _id << " = ";
            _value->translate(tabs, context);
            std::cout << std::endl;
        }
        else if ((type != "") && (!context.isParameter(_id)))
        {
            std::cout << _id << " = 0" << std::endl;
        }
        else
        {
            std::cout << _id;
        }
    }

    std::string compile(Context &context)
    {
        if (type == "")
        {
            int offset = context.symbol_table[_id]->getOffset();
            std::string type = context.symbol_table[_id]->getType();
            if (offset != -1)
            {
                if (type == "float")
                {
                    std::cout << "\t l.s $f0," << offset << "($fp)" << std::endl;
                    std::cout << "\t nop" << std::endl;
                }
                else if (type == "double")
                {
                    std::cout << "\t l.s $f0," << offset << "($fp)" << std::endl;
                    std::cout << "\t nop" << std::endl;
                    std::cout << "\t l.s $f1," << offset + 4 << "($fp)" << std::endl;
                    std::cout << "\t nop" << std::endl;
                }
                else
                {
                    std::cout << "\t"
                              << "lw $2," << offset << "($fp)" << std::endl;
                    std::cout << "\t nop" << std::endl;
                }
            }
            else
            {
                std::cout << "\t"
                          << "lui     $t2,%hi(" << _id << ")" << std::endl;
                std::cout << "\t nop" << std::endl;
                std::cout << "\t"
                          << "addiu $t2, $t2, %lo(" << _id << ")" << std::endl;
                if (type == "float")
                {
                    std::cout << "\tl.s $f0,0($t2)" << std::endl;
                    std::cout << "\t nop" << std::endl;
                }
                else if (type == "double")
                {
                    std::cout << "\tl.s $f0,0($t2)" << std::endl;
                    std::cout << "\t nop" << std::endl;
                    std::cout << "\tl.s $f1,4($t2)" << std::endl;
                    std::cout << "\t nop" << std::endl;
                }
                else
                {
                    std::cout << "\tlw $2,0($t2)" << std::endl;
                    std::cout << "\t nop" << std::endl;
                }
            }
            return type;
        }
        else if (context.isglobal())
        {
            context.addGlobalVariable(_id, type);
            std::cout << ".globl " << _id << std::endl;
            std::cout << ".data" << std::endl;
            std::cout << ".align 2" << std::endl;
            std::cout << _id << ":" << std::endl;
            if (_value != nullptr)
            {
                if (type == "float")
                {
                    std::cout << "\t.float " << std::static_pointer_cast<FConstant>(_value)->getValue() << std::endl;
                }
                else if (type == "double")
                {
                    double tmp = std::static_pointer_cast<DConstant>(_value)->getValue();
                    std::cout << "\t.double " << tmp << std::endl;
                }
                else
                    std::cout << "\t.word " << std::static_pointer_cast<Constant>(_value)->getValue() << std::endl;
            }
            else
            {
                std::cout << "\t.word 0" << std::endl;
                if (type == "double")
                    std::cout << "\t.word 0" << std::endl;
            }
            return type;
        }
        else
        {
            int offset = context.addVariable(_id, type);
            if (_value != nullptr)
            {
                _value->compile(context);
                if (type == "float")
                {
                    std::cout << "\t"
                              << "s.s $f0," << offset << "($fp)" << std::endl;
                    std::cout << "\t nop" << std::endl;
                }
                else if (type == "double")
                {

                    std::cout << "\t"
                              << "s.s $f0," << offset << "($fp)" << std::endl;
                    std::cout << "\t nop" << std::endl;
                    std::cout << "\t"
                              << "s.s $f1," << offset + 4 << "($fp)" << std::endl;
                    std::cout << "\t nop" << std::endl;
                }
                else
                {
                    std::cout << "\t"
                              << "sw $2," << offset << "($fp)" << std::endl;
                    std::cout << "\t nop" << std::endl;
                }
            }
            return type;
        }
    }

    void assign(Context &context, std::string &type) override
    {
        int offset = std::static_pointer_cast<Record>(context.symbol_table[_id])->getOffset();
        if (offset != -1)
        {
            if (type == "float")
            {
                std::cout << "\t s.s $f0," << offset << "($fp)" << std::endl;
            }
            else if (type == "double")
            {
                std::cout << "\t s.s $f0," << offset << "($fp)" << std::endl;
                std::cout << "\t s.s $f1," << offset + 4 << "($fp)" << std::endl;
            }
            else
            {
                std::cout << "\tsw $2," << offset << "($fp)" << std::endl;
            }
        }
        else
        {
            std::cout << "\t"
                      << "lui     $t2,%hi(" << _id << ")" << std::endl;
            std::cout << "\t"
                      << "addiu $t2, $t2, %lo(" << _id << ")" << std::endl;
            std::cout << "\tsw $2,0($t2)" << std::endl;
        }
    }

    void getAddress(Context &context) const override
    {
        int offset = context.symbol_table[_id]->getOffset();
        if (offset != -1)
        {
            std::cout << "\t"
                      << "addi $2,"
                      << "$fp," << offset << std::endl;
        }
        else
        {
            std::cout << "\t"
                      << "lui     $t2,%hi(" << _id << ")" << std::endl;
            std::cout << "\t"
                      << "addiu $t2, $t2, %lo(" << _id << ")" << std::endl;
            std::cout << "\t"
                      << "\tadd $2,$0,$t2" << std::endl;
        }
    }
};
class Array : public Declaration
{
    // Storage on stack[3][2]:
    //     |-____________
    //     |  0,0
    //     | 0,1
    //     | 1,0
    //     |1,1
    //     |2,0
    //     |2,1

public:
    std::vector<int> dimensions;
    std::shared_ptr<Initializer> init_list = nullptr;

    Array(std::string id, std::string type, int size) : Declaration(id, type, "array"), dimensions({size}){};
    Array(std::string id, std::string type, std::shared_ptr<Initializer> i) : Declaration(id, type, "array"), init_list(i){};

    std::string compile(Context &context)
    {
        if (init_list == nullptr)
        {
            if (context.isglobal())
            {
                std::cout << ".globl " << _id << std::endl;
                std::cout << ".data" << std::endl;
                std::cout << ".align 2" << std::endl;
                std::cout << _id << ":" << std::endl;
                int sum = std::accumulate(dimensions.begin(), dimensions.end(), 1,std::multiplies<int>());
                for (int i = 0; i < sum; i++)
                {
                    std::cout << "\t"
                              << ".word 0" << std::endl;
                }
                context.symbol_table[_id] = std::make_shared<ArrayRecord>(_id, -1, dimensions);
            }
            else
            {
                context.addArray(_id, "int", dimensions);
            }
        }
        else
        {

            std::vector<int> array = {};
            init_list->initialize(array, dimensions, 0);

            if (context.isglobal())
            {
                std::cout << ".globl " << _id << std::endl;
                std::cout << ".data" << std::endl;
                std::cout << ".align 2" << std::endl;
                std::cout << _id << ":" << std::endl;
                for (int i = 0; i < array.size(); i++)
                {
                    std::cout << "\t"
                              << ".word " << array[i] << std::endl;
                }
                context.symbol_table[_id] = std::make_shared<ArrayRecord>(_id, -1, dimensions);
            }
            else
            {
                context.addArray(_id, "int", dimensions, array);
            }
        }
        return type;
    }
};

class ArrayCall : public Identifiable, public Assignable
{

public:
    std::vector<ExpressionPtr> number;
    ArrayCall(std::string id, ExpressionPtr n) : Identifiable(id), number({n}){};
    std::string compile(Context &context) override
    {
        calculateOffset(context);
        int offset = context.symbol_table[_id]->getOffset();
        if (offset != -1)
        {
            std::cout << "\t"
                      << "addi $2,$0," << offset << std::endl;
            std::cout << "\t"
                      << "add $2,$fp,$2" << std::endl;
            if (context.symbol_table[_id]->getRecordType() == "pointer")
                std::cout << "\tlw $2,0($2)" << std::endl;
            std::cout << "\t"
                      << "add $2,$t0,$2" << std::endl;
            std::cout << "\t"
                      << "lw $2,0($2)" << std::endl;
        }
        else
        {
            std::cout << "\t"
                      << "lui     $2,%hi(" << _id << ")" << std::endl;
            std::cout << "\t"
                      << "addiu $2, $2, %lo(" << _id << ")" << std::endl;
            if (context.symbol_table[_id]->getRecordType() == "pointer")
                std::cout << "\tlw $2,0($2)" << std::endl;
            std::cout << "\t"
                      << "sub $2,$2,$t0" << std::endl;
            std::cout << "\t"
                      << "lw $2,0($2)" << std::endl;
        }
        return context.symbol_table[_id]->getType();
    }
    void calculateOffset(Context &context) const
    {
        std::cout << "\t add $t0,$0,$0" << std::endl;
        if (context.symbol_table[_id]->getRecordType() == "pointer")
        {
            number[0]->compile(context);
            std::cout << "\taddi $t0,$0,4" << std::endl;
            std::cout << "\tmul $t0,$2,$t0" << std::endl;
        }
        else
        {
            std::vector<int> &dim = std::static_pointer_cast<ArrayRecord>(context.symbol_table[_id])->dimensions;
            for (int i = 0; i < number.size(); i++)
            {
                int offset = std::accumulate(dim.begin() + i + 1, dim.end(), 1, std::multiplies<int>());
                context.push_stack();
                std::cout << "\t"
                          << "sw $t0,0($sp)" << std::endl;
                number[i]->compile(context);
                std::cout << "\t"
                          << "lw $t0,0($sp)" << std::endl;
                context.pop_stack();
                std::cout << "\t"
                          << "addi $t1,$0," << -(offset * 4) << std::endl;
                std::cout << "\tmul $t1,$t1,$2" << std::endl;
                std::cout << "\tadd $t0,$t0,$t1" << std::endl;
            }
        }
    }
    void assign(Context &context, std::string &type) override
    {
        context.push_stack();
        std::cout << "\t sw $2,0($sp)" << std::endl;
        calculateOffset(context);
        int offset = context.symbol_table[_id]->getOffset();
        if (offset != -1)
        {
            std::cout << "\t"
                      << "addi $2,$0," << offset << std::endl;
            std::cout << "\t"
                      << "add $2,$fp,$2" << std::endl;
            if (context.symbol_table[_id]->getRecordType() == "pointer")
                std::cout << "\tlw $2,0($2)" << std::endl;
            std::cout << "\t"
                      << "add $2,$t0,$2" << std::endl;
            std::cout << "\t lw $t2,0($sp)" << std::endl;
            context.pop_stack();
            std::cout << "\t"
                      << "sw $t2,0($2)" << std::endl;
            std::cout << "\tadd $2,$0,$t2" << std::endl;
        }
        else
        {
            std::cout << "\t"
                      << "lui     $2,%hi(" << _id << ")" << std::endl;
            std::cout << "\t"
                      << "addiu $2, $2, %lo(" << _id << ")" << std::endl;
            if (context.symbol_table[_id]->getRecordType() == "pointer")
                std::cout << "\tlw $2,0($2)" << std::endl;
            std::cout << "\t"
                      << "sub $2,$2,$t0" << std::endl;
            std::cout << "\t lw $t2,0($sp)" << std::endl;
            context.pop_stack();
            std::cout << "\t"
                      << "sw $t2,0($2)" << std::endl;
            std::cout << "\tadd $2,$0,$t2" << std::endl;
        }
    }

    void getAddress(Context &context) const override
    {
        calculateOffset(context);
        int offset = context.symbol_table[_id]->getOffset();
        if (offset != -1)
        {
            std::cout << "\t"
                      << "addi $2,$0," << offset << std::endl;
            std::cout << "\t"
                      << "add $2,$fp,$2" << std::endl;
            if (context.symbol_table[_id]->getRecordType() == "pointer")
                std::cout << "\tlw $2,0($2)" << std::endl;
        }
        else
        {
            std::cout << "\t"
                      << "lui     $2,%hi(" << _id << ")" << std::endl;
            std::cout << "\t"
                      << "addiu $2, $2, %lo(" << _id << ")" << std::endl;
            if (context.symbol_table[_id]->getRecordType() == "pointer")
                std::cout << "\tlw $2,0($2)" << std::endl;
            std::cout << "\t"
                      << "sub $2,$2,$t0" << std::endl;
        }
    }
};

class Pointer : public Declaration
{
protected:
    std::shared_ptr<Expression> init;

public:
    int point_count;
    Pointer(std::string id, ExpressionPtr e, std::string t, int p) : Declaration(id, t, "pointer"), init(e), point_count(p){};
    Pointer(const std::shared_ptr<Pointer> old) : Declaration(old->_id, old->type, "pointer"), init(old->init), point_count(old->point_count){};
    Pointer(const std::shared_ptr<Pointer> old, std::shared_ptr<Expression> i) : Declaration(old->_id, old->type, "pointer"), init(i), point_count(old->point_count){};
    std::string compile(Context &context) override
    {
        if (context.isglobal())
        {
            context.addGlobalPointer(type, _id);
            std::cout << ".globl " << _id << std::endl;
            std::cout << ".data" << std::endl;
            std::cout << ".align 2" << std::endl;
            std::cout << _id << ":" << std::endl;
        }
        else
        {
            int offset = context.addPointer(type, _id);
            if (init != nullptr)
            {
                init->compile(context);
                std::cout << "\t"
                          << "sw $2," << offset << "($fp)" << std::endl;
            }
        }
        return type;
    }
};

class FuncPointer : public Pointer
{
private:
    int params_num;

public:
    FuncPointer(std::string id, ExpressionPtr e, std::string t, int p, int pa) : Pointer(id, e, t, p), params_num(pa){};
    FuncPointer(const std::shared_ptr<Pointer> old, int p) : Pointer(*old), params_num(p){};
    FuncPointer(const std::shared_ptr<Pointer> old, int p, ExpressionPtr i) : Pointer(*old), params_num(p) { init = i; };
    std::string compile(Context &context) override
    {
        int offset = context.addFuncPointer(type, _id, params_num);
        if (init != nullptr)
        {
            init->compile(context);
            std::cout << "\tsw $2," << offset << "($fp)" << std::endl;
        }
        return type;
    }
    int getParams()
    {
        return params_num;
    }
};

#endif // !AST_PRIMITIVES_H
