#include "utilities.h"
#include <windows.h>

namespace Utilities
{
    void GetScreenSize(int &x, int &y)
    {
        x = GetSystemMetrics(SM_CXSCREEN);
        y = GetSystemMetrics(SM_CYSCREEN);
    }

    void ShowMsgBox(const std::string &title, const std::string &message)
    {
        MessageBox(
            NULL,
            message.data(),
            title.data(),
            0);
    }
}