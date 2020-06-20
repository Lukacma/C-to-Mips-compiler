#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <vector>

class Declaration;
class Record
{
protected:
    std::string recordType;
    std::string type;
    int offset;

public:
    Record(std::string rT, std::string t, int o) : recordType(rT), type(t), offset(o){};

    std::string getRecordType() const
    {
        return recordType;
    }

    std::string getType() const
    {
        return type;
    }
    int getOffset() const
    {
        return offset;
    }
};

class VariableRecord : public Record
{

public:
    VariableRecord(std::string t, int o) : Record("var", t, o) {}
};

class PointerRecord : public Record
{
private:
    int p_count;

public:
    PointerRecord(std::string t, int o) : Record("pointer", t, o){};
};

class FuncPointerRecord : public Record
{
private:
    int p_count;
    int numOfParams;

public:
    FuncPointerRecord(std::string t, int o, int n) : Record("funcpointer", t, o), numOfParams(n){};
    int getNumOfParams()
    {
        return numOfParams;
    }
};

class ArrayRecord : public Record
{
public:
    std::vector<int> dimensions;

    ArrayRecord(std::string t, int o, std::vector<int> d) : Record("array", t, o), dimensions(d) {}
};

class FunctionRecord : public Record
{
public:
    std::vector<std::shared_ptr<Declaration>> params;
    FunctionRecord(std::string t, std::vector<std::shared_ptr<Declaration>> &n) : Record("func", t, -1), params(n) {}
    int getNumOfParams()
    {
        return params.size();
    }
};

typedef std::shared_ptr<Record> RecordPtr;

#endif