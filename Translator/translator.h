#ifndef TRANSLATOR_CONTEXT_H
#define TRANSLATOR_CONTEXT_H

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>

class TranslatorContext
{
private:
    std::unordered_set<std::string> parameters;

public:
    std::vector<std::string> globalVars;

    TranslatorContext() : parameters({}), globalVars({}){};
    //TranslatorContext(std::unordered_map<std::string, bool> gv, std::unordered_set<std::string> dv) : global_variables(gv), declared_variables(dv){};
    //TranslatorContext(const TranslatorContext &old) : global_variables(old.global_variables), declared_variables(old.declared_variables){};

    void addGlobalVar(std::string name)
    {
        globalVars.push_back(name);
    }

    void addParameter(std::string name)
    {
        parameters.insert(name);
    }

    bool isParameter(std::string name)
    {
        return parameters.count(name);
    }

    void clearParameters()
    {
        parameters.clear();
    }
};

#endif // !TRANSLATOR_CONTEXT_H