#include "pyglober.h"
#include <iostream>
int main()
{
    if (PyRTInit())
    {
        char **result;
        int resultCount;

        if (PyGlobber("**/*.pyc", NULL, NULL, 1, 0, &result, &resultCount) && resultCount)
        {
            std::cout << "CC files found!" << '\n';
            for (int i = 0; i < resultCount; i++)
            {
                std::cout << result[i] << '\n';
            }
        }
        else
        {
            std::cout << "CC files NOT found or execute failed!" << '\n';
        }
    }

    PYRTFinal();

    return 0;
}