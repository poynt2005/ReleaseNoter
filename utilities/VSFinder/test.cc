#include "VSFinder.h"

#include <vector>
#include <map>
#include <string>
#include <iostream>

int main()
{
    std::vector<std::map<std::string, val>> vsInfo;

    if (GetVsInfo(vsInfo))
    {
        for (auto &&t : vsInfo)
        {
            for (auto &&m : t)
            {
                std::cout << m.first << ' ';

                if (m.second.valType == STRING_TYPE)
                {
                    std::cout << m.second.stringVal << '\n';
                }
            }
        }
    }
    std::cout << "EndEnd\n";
    return 0;
}