#include "parser.tab.hpp"
#include "translator.h"

int main()
{
    std::vector<ExpressionPtr> ret = parseAST();
    TranslatorContext context;

    for (auto el : ret)
    {   
        el->translate(0, context);
        context.clearParameters();

    }

    std::cout << "if __name__ == \"__main__\":" << std::endl;
    std::cout << "\timport sys\n\tret=main()\n\tsys.exit(ret)" << std::endl;
    return 0;
}