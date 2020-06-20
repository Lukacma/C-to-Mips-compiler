#ifndef CONTEXT_H
#define CONTEXT_H

#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <numeric>
#include "Record.h"

class Context
{
private:
    int offset; //offset points to empty elements so one word lower than sp
    std::string bflag = "", cflag = "";
    bool isGlobal = false;

public:
    std::vector<std::string> caseflags = {};

    std::unordered_map<std::string, RecordPtr> symbol_table;

    Context() : offset(1), isGlobal(true), symbol_table({}){};
    Context(const Context &old) : offset(old.offset), bflag(old.getBflag()), cflag(old.getCflag()), symbol_table(old.symbol_table){};
    Context(std::string b, std::string c, Context &cont) : offset(cont.offset), bflag(b), cflag(c), symbol_table(cont.symbol_table){};
    int addVariable(std::string name, std::string type)
    {
        if (type == "double")
            push_stack();
        symbol_table[name] = std::make_shared<VariableRecord>(type, -(offset * 4));
        //-(offset * 4);

        push_stack();
        return -((offset - 1) * 4);
    }

    int addGlobalVariable(std::string name, std::string type)
    {

        symbol_table[name] = std::make_shared<VariableRecord>(type, -1);

        return -1;
    }

    void addFunction(std::string name, std::string type, std::vector<std::shared_ptr<Declaration>> &par)
    {
        symbol_table[name] = std::make_shared<FunctionRecord>(type, par);
    }

    int addArray(std::string name, std::string type, std::vector<int> &dim, const std::vector<int> &init_list = {})
    {
        symbol_table[name] = std::make_shared<ArrayRecord>(type, -(offset * 4), dim);
        int sum = std::accumulate(dim.begin(), dim.end(), 1,std::multiplies<int>());
        for (int i = 0; i < sum; i++)
        {
            push_stack();
            if (init_list.size() != 0)
            {
                std::cout << "\t"
                          << "addi $2,$0," << init_list[i] << std::endl;
                std::cout << "\t"
                          << "sw $2,0($sp)" << std::endl;
            }
            else
            {
                std::cout << "\tsw $0,0($sp)" << std::endl;
            }
        }
        return -((offset - sum) * 4);
    }

    int addPointer(const std::string &type, const std::string &id)
    {

        auto tmp = std::make_shared<PointerRecord>(type, -(offset * 4));
        push_stack();
        symbol_table[id] = tmp;
        return -((offset - 1) * 4);
    }

    int addGlobalPointer(const std::string &type, const std::string &id)
    {

        auto tmp = std::make_shared<PointerRecord>(type, -1);
        symbol_table[id] = tmp;
        return -1;
    }

    int addFuncPointer(const std::string &type, const std::string &id, int num)
    {
        auto tmp = std::make_shared<FuncPointerRecord>(type, -(offset * 4), num);
        push_stack();
        symbol_table[id] = tmp;
        return -(offset - 1) * 4;
    }
    void push_stack()
    {
        offset++;
        std::cout << "\t"
                  << "addiu $sp,$sp,-4" << std::endl;
    }
    void pop_stack()
    {
        offset--;
        std::cout << "\t"
                  << "addiu $sp,$sp,4" << std::endl;
    }
    std::string getBflag() const
    {
        return bflag;
    }
    std::string getCflag() const
    {
        return cflag;
    }
    bool isglobal()
    {
        return isGlobal;
    }
};

#endif // !CONTEXT_H