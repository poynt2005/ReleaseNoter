#include <iostream>
#include <windows.h>

int main()
{
    int xScren = GetSystemMetrics(SM_CXSCREEN);
    int yScren = GetSystemMetrics(SM_CYSCREEN);

    std::cout << xScren << " ," << yScren << "\n";

    MessageBox(
        NULL,
        "EFWEF",
        "EFWEF",
        0);

    return 0;
}