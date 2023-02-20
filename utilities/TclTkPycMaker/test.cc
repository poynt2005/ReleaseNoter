#include <iostream>
#include <TclTkPycMaker.h>

void printCodeFunc(int codeType, int code)
{
    char *codeInfo = NULL;

    if (CodeMapping(codeType, code, &codeInfo))
    {
        std::cout << codeInfo << '\n';
    }
}

int main()
{

    if (MakeTclTkPycFiles(printCodeFunc))
    {
        std::cout << "Process success\n";
    }
    else
    {
        std::cout << "Process failed\n";
    }

    std::cout << "End\n";

    return 1;
}