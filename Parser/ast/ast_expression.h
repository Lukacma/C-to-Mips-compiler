#ifndef AST_EXPRESSION_H
#define AST_EXPRESSION_H
#include <iostream>
#include <memory>
#include "Context.h"
#include "translator.h"
#include <string>

class Expression
{
public:
    virtual void print(){};
    virtual void translate(int tabs, TranslatorContext &context){};
    virtual std::string compile(Context &context) = 0;
};
typedef std::shared_ptr<Expression> ExpressionPtr;

class Identifiable : public Expression
{
protected:
    std::string _id;

public:
    Identifiable(std::string id) : _id(id){};
    Identifiable() : _id(""){};
    virtual std::string getId()
    {
        return _id;
    }
    virtual void getAddress(Context &context) const = 0;
};

class Declaration : public Identifiable
{

public:
    std::string type;
    std::string classType;
    Declaration(std::string id, std::string c) : Identifiable(id), type(""), classType(c){};
    Declaration(std::string id, std::string t, std::string c) : Identifiable(id), type(t), classType(c){};
    std::string compile(Context &context){return "";};
    void getAddress(Context &context) const override{};
};

class Assignable
{
public:
    virtual void assign(Context &context, std::string &type) = 0;
};
#endif // !AST_EXPRESSION.H
