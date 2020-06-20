#ifndef AST_INITIALIZER_H
#define AST_INITIALIZER_H

#include <vector>
#include <string>
#include <numeric>

class Initializer
{
private:
    bool _isList = false;
    int _value;

public:
    std::vector<std::shared_ptr<Initializer>> init;
    Initializer(std::vector<std::shared_ptr<Initializer>> i, bool t) : _isList(t), init(i){};
    Initializer(int val) : _value(val){};
    int getVal()
    {
        return _value;
    }
    void initialize(std::vector<int> &array, std::vector<int> &dim, int cur)
    {
        for (int i = 0; i < init.size(); i++)
        {
            if (!init[i]->_isList)
            {
                array.push_back(init[i]->getVal());
            }
            else
            {
                std::vector<int> tmp;
                init[i]->initialize(tmp, dim, cur + 1);
                array.insert(array.end(), tmp.begin(), tmp.end());
            }
        }
        int size_required = std::accumulate(std::next(dim.begin(), cur), dim.end(), 1, std::multiplies<int>());
        array.insert(array.end(), size_required - array.size(), 0);
    }
};

#endif // !AST_INITIALIZER_H