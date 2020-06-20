#ifndef AST_FUNCTION_H
#define AST_FUNCTION_H
#include "ast_expression.h"
#include "ast_statement.h"
#include <string>
#include <algorithm>

class Function : public Declaration
{
private:
    StatementPtr _body;
    std::vector<std::shared_ptr<Declaration>> params;

    void storeOnStack(Context &context)
    {
        int stack_offset = 0;
        bool float_available = true;
        for (int i = 0; i < params.size(); i++)
        {
            addToTable(context, params[i], stack_offset);
            if (i == 2)
                float_available = false;

            if (stack_offset <= 3)
            {
                bool isFloatType = params[i]->type == "float";
                bool isDoubleType = params[i]->type == "double";

                if (isFloatType && float_available)
                {
                    std::cout << "\t"
                              << "s.s $f" << 12 + i * 2 << "," << 8 + stack_offset * 4 << "($fp)" << std::endl;
                    stack_offset++;
                }
                else if (isDoubleType && float_available)
                {
                    std::cout << "\t"
                              << "s.s $f" << 12 + i * 2 << "," << 8 + stack_offset * 4 << "($fp)" << std::endl;
                    std::cout << "\t"
                              << "s.s $f" << 12 + (i * 2) + 1 << "," << 8 + (stack_offset + 1) * 4 << "($fp)" << std::endl;
                    stack_offset += 2;
                }
                else if (isDoubleType)
                {
                    std::cout << "\t"
                              << "sw $" << 4 + stack_offset << "," << 8 + stack_offset * 4 << "($fp)" << std::endl;
                    std::cout << "\t"
                              << "sw $" << 4 + stack_offset + 1 << "," << 8 + (stack_offset + 1) * 4 << "($fp)" << std::endl;
                    stack_offset += 2;
                }
                else
                {
                    float_available = false;
                    std::cout << "\t"
                              << "sw $" << 4 + stack_offset << "," << 8 + stack_offset * 4 << "($fp)" << std::endl;
                    stack_offset++;
                }
            }
            else
            {
                if (params[i]->type == "double")
                    stack_offset += 2;
                else
                    stack_offset++;
            }
        }
    }

    void loadFromStack(Context &context)
    {
        int stack_offset = 0;
        bool float_available = true;
        for (int i = 0; i < params.size(); i++)
        {

            if (i == 2)
                float_available = false;

            if (stack_offset <= 3)
            {
                bool isFloatType = params[i]->type == "float";
                bool isDoubleType = params[i]->type == "double";

                if (isFloatType && float_available)
                {
                    std::cout << "\t"
                              << "l.s $f" << 12 + i * 2 << "," << 8 + stack_offset * 4 << "($fp)" << std::endl;
                    stack_offset++;
                }
                else if (isDoubleType && float_available)
                {
                    std::cout << "\t"
                              << "l.s $f" << 12 + i * 2 << "," << 8 + stack_offset * 4 << "($fp)" << std::endl;
                    std::cout << "\t"
                              << "l.s $f" << 12 + (i * 2) + 1 << "," << 8 + (stack_offset + 1) * 4 << "($fp)" << std::endl;
                    stack_offset += 2;
                }
                else if (isDoubleType)
                {
                    std::cout << "\t"
                              << "lw $" << 4 + stack_offset << "," << 8 + stack_offset * 4 << "($fp)" << std::endl;
                    std::cout << "\t"
                              << "lw $" << 4 + stack_offset + 1 << "," << 8 + (stack_offset + 1) * 4 << "($fp)" << std::endl;
                    stack_offset += 2;
                }
                else
                {
                    float_available = false;
                    std::cout << "\t"
                              << "lw $" << 4 + stack_offset << "," << 8 + stack_offset * 4 << "($fp)" << std::endl;
                    stack_offset++;
                }
            }
            else
            {
                if (params[i]->type == "double")
                    stack_offset += 2;
                else
                    stack_offset++;
            }
        }
    }
    void addToTable(Context &context, std::shared_ptr<Declaration> param, int stack_offset)
    {
        if (param->classType == "pointer")
            context.symbol_table[param->getId()] = std::make_shared<PointerRecord>(param->type, 8 + stack_offset * 4);
        else if (param->classType == "funcpointer")
        {
            int n = std::static_pointer_cast<FuncPointer>(param)->getParams();
            context.symbol_table[param->getId()] = std::make_shared<FuncPointerRecord>(param->type, 8 + stack_offset * 4, n);
        }
        else if (param->classType == "var")
            context.symbol_table[param->getId()] = std::make_shared<VariableRecord>(param->type, 8 + stack_offset * 4);
        else if (param->classType == "array")
            context.symbol_table[param->getId()] = std::make_shared<ArrayRecord>(param->type, 8 + stack_offset * 4, std::static_pointer_cast<Array>(param)->dimensions);
    }

public:
    Function(std::string id, StatementPtr body, std::string ret) : Declaration(id, ret, "func"), _body(body), params({})
    {
    }
    Function(std::string id, StatementPtr body, std::string ret, std::vector<std::shared_ptr<Declaration>> p) : Declaration(id, ret, "func"), _body(body), params(p) {}

    Function(const Function &old) : Declaration(old.type, old.type), _body(old.getBody()) {}

    StatementPtr getBody() const
    {
        return _body;
    }
    std::vector<std::shared_ptr<Declaration>> getParams()
    {
        return params;
    }
    void print()
    {
        std::cout << "[ " << type << " " << this->type << " (";
        for (auto el : params)
        {
            std::cout << el << " ";
        }
        std::cout << ")" << std::endl;
        if (_body != nullptr)
        {
            _body->print();
        }
        std::cout << " ]";
    }

    void translate(int tabs, TranslatorContext &context)
    {
        std::cout << std::string(tabs, '\t') << "def " << _id << "(";
        bool firstIter = true;
        for (auto el : params)
        {
            if (!firstIter)
                std::cout << ", ";
            std::cout << el->getId();
            context.addParameter(el->getId());
            firstIter = false;
        }

        std::cout << "):" << std::endl;

        if (_body != nullptr)
        {
            std::cout << std::string(tabs+1, '\t') << "pass" << std::endl;
            for (auto el : context.globalVars)
            {
                std::cout << std::string(tabs + 1, '\t') << "global " << el << std::endl;
            }
            _body->translate(tabs + 1, context);
        }
        else
        {
            std::cout << std::string(tabs+1, '\t') << "pass" << std::endl;
        }
    }

    std::string compile(Context &context)
    {

        context.addFunction(_id, type, params);

        if (_body != nullptr)
        {
            std::cout << ".text" << std::endl;
            std::cout << ".globl " << _id << std::endl;
            std::cout << ".align 2" << std::endl;
            std::cout << _id << ":" << std::endl;
            std::cout << "\t"
                      << "addiu "
                      << "$sp,$sp,-4" << std::endl;
            std::cout << "\t"
                      << "sw $31,0($sp)" << std::endl;
            std::cout << "\t"
                      << "addiu "
                      << "$sp,$sp,-4" << std::endl;
            std::cout << "\t"
                      << "sw $fp,0($sp)" << std::endl;
            std::cout << "\t"
                      << "move $fp, $sp" << std::endl;
            Context new_context(context);
            storeOnStack(new_context);
            _body->compile(new_context);
            loadFromStack(new_context);
            std::cout << "\t"
                      << "move $sp,$fp" << std::endl;
            std::cout << "\t"
                      << "lw $fp, 0($sp)" << std::endl;
            std::cout << "\t"
                      << "lw $31, 4($sp)" << std::endl;
            std::cout << "\t"
                      << "addiu $sp, $sp, 8" << std::endl;
            std::cout << "\t"
                      << "jr $31" << std::endl;
        }
        return "";
    }
};

class FunctionCall : public Identifiable
{
private:
    std::vector<ExpressionPtr> params;

    void saveParams(std::string &type)
    {
        std::cout << "\t add $sp,$sp,-32" << std::endl;
        std::cout << "\t sw $4,0($sp)" << std::endl;
        std::cout << "\t sw $5,4($sp)" << std::endl;
        std::cout << "\t sw $6,8($sp)" << std::endl;
        std::cout << "\t sw $7,12($sp)" << std::endl;
        if (type != "int")
        {
            std::cout << "\t s.s $f12,16($sp)" << std::endl;
            std::cout << "\t s.s $f13,20($sp)" << std::endl;
            std::cout << "\t s.s $f14,24($sp)" << std::endl;
            std::cout << "\t s.s $f15,28($sp)" << std::endl;
        }
    }
    void loadParams(std::string &type)
    {
        std::cout << "\t lw $4,0($sp)" << std::endl;
        std::cout << "\t lw $5,4($sp)" << std::endl;
        std::cout << "\t lw $6,8($sp)" << std::endl;
        std::cout << "\t lw $7,12($sp)" << std::endl;
        if (type != "int")
        {
            std::cout << "\t l.s $f12,16($sp)" << std::endl;
            std::cout << "\t l.s $f13,20($sp)" << std::endl;
            std::cout << "\t l.s $f14,24($sp)" << std::endl;
            std::cout << "\t l.s $f15,28($sp)" << std::endl;
        }
        std::cout << "\t add $sp,$sp,32" << std::endl;
    }
    void loadParameters(Context &context)
    {
        int stack_offset = 0;
        bool float_available = true;
        int number_of_params;
        if (context.symbol_table[_id]->getRecordType() == "funcpointer")
            number_of_params = std::static_pointer_cast<FuncPointerRecord>(context.symbol_table[_id])->getNumOfParams();
        else
            number_of_params = std::static_pointer_cast<FunctionRecord>(context.symbol_table[_id])->getNumOfParams();

        std::cout << "\t"
                  << "addi $sp,$sp,-16" << std::endl;
        for (int i = 0; i < number_of_params; i++)
        {

            if (stack_offset <= 3)
            {
                std::string param_type;
                if (context.symbol_table[_id]->getRecordType() == "funcpointer")
                    param_type = "int";
                else
                    param_type = std::static_pointer_cast<FunctionRecord>(context.symbol_table[_id])->params[i]->type;
                bool isFloatType = param_type == "float";
                bool isDoubleType = param_type == "double";
                saveParams(param_type);
                if (i < params.size())
                    params[i]->compile(context);
                loadParams(param_type);
                if (i == 2)
                    float_available = false;

                if (isFloatType && float_available)
                {
                    std::cout << "\t"
                              << "mov.s $f" << 12 + i * 2 << ",$f0" << std::endl;
                    stack_offset++;
                }
                else if (isDoubleType && float_available)
                {
                    std::cout << "\t"
                              << "mov.s $f" << 12 + i * 2 << ",$f0" << std::endl;
                    std::cout << "\t"
                              << "mov.s $f" << 12 + (i * 2) + 1 << ",$f1" << std::endl;
                    stack_offset += 2;
                    context.push_stack();
                }
                else if (isDoubleType)
                {
                    std::cout << "\t"
                              << "mfc1 $" << 4 + stack_offset << ",$f0" << std::endl;
                    std::cout << "\t"
                              << "mfc1 $" << 4 + stack_offset + 1 << ",$f1" << std::endl;
                    stack_offset += 2;
                    context.push_stack();
                }
                else if (isFloatType)
                {
                    std::cout << "\t"
                              << "mfc1 $" << 4 + stack_offset << ",$f0" << std::endl;
                    stack_offset++;
                }

                else
                {
                    float_available = false;
                    std::cout << "\t"
                              << "add $" << 4 + stack_offset << ",$0,$2" << std::endl;
                    stack_offset++;
                }
            }
            else //run out of registers
            {
                number_of_params = i;
                break;
            }
        }
        for (int j = params.size() - 1; j >= number_of_params; j--)
        {
            std::string param_type;
            if (context.symbol_table[_id]->getRecordType() == "funcpointer")
                param_type = "int";
            else
                param_type = std::static_pointer_cast<FunctionRecord>(context.symbol_table[_id])->params[j]->type;
            bool isFloatType = param_type == "float";
            bool isDoubleType = param_type == "double";
            saveParams(param_type);
            params[j]->compile(context);
            loadParams(param_type);
            context.push_stack();

            if (isDoubleType)
            {
                std::cout << "\t"
                          << "s.s $f1,16($sp)" << std::endl;
                context.push_stack();
                std::cout << "\t"
                          << "s.s $f0,16($sp)" << std::endl;
            }
            else if (isFloatType)
            {
                std::cout << "\t"
                          << "s.s $f0,16($sp)" << std::endl;
            }
            std::cout << "\t"
                      << "sw  $2,16($sp)" << std::endl;
        }
    }

public:
    FunctionCall(std::string id, std::vector<ExpressionPtr> p) : Identifiable(id), params(p){};
    FunctionCall(std::string id) : Identifiable(id){};
    void translate(int tabs, TranslatorContext &context)
    {
        std::cout << _id << "(";
        bool firstIter = true;
        for (auto el : params)
        {
            if (!firstIter)
                std::cout << ", ";
            el->translate(tabs, context);
            firstIter = false;
        }
        std::cout << ")";
    }

    virtual std::string compile(Context &context)
    {
        loadParameters(context);
        if (context.symbol_table[_id]->getRecordType() == "funcpointer")
        {
            int offset = context.symbol_table[_id]->getOffset();
            if (offset == -1)
            {
                getAddress(context);
                std::cout << "\tlw $2,0($2)" << std::endl;
            }
            else
                std::cout << "\tlw $2," << offset << "($fp)" << std::endl;
            std::cout << "\t"
                      << "jalr $2" << std::endl;
        }
        else
        {
            std::cout << "\t"
                      << "jal " << _id << std::endl;
        }
        for (int i = 0; i < std::max((int)params.size(), 4); i++)
        {
            context.pop_stack();
        }

        return context.symbol_table[_id]->getType();
    };

    void getAddress(Context &context) const override
    {
        std::cout << "\t"
                  << "lui     $2,%hi(" << _id << ")" << std::endl;
        std::cout << "\t"
                  << "addiu $2, $2, %lo(" << _id << ")" << std::endl;
    }
};
#endif // !AST_FUNCTION_H
