#include "vc++/DependenciesFinder/DependenciesFinder.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        printf("No managed dll to check\n");
        return 1;
    }

    char **dependencies;
    int size = 0;

    int returnCode = Finder(argv[1], &dependencies, &size);

    if (!returnCode)
    {
        printf("Check dll to failed\n");
        return 1;
    }
    else
    {
        printf("Check dll success\n");
        printf("-************-\n");
        printf("Find %d number of dependencies\n", size);

        for (int i = 0; i < size; i++)
        {
            printf("%s\n", dependencies[i]);
        }

        printf("-************-\n");
        return 0;
    }

    return 0;
}