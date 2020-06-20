#include "Context.h"
#include "parser.tab.hpp"

int main()
{
    Context context;
    std::vector<ExpressionPtr> ret = parseAST();
    for (auto el : ret)
    {
        el->compile(context);
    }
    return 0;
}