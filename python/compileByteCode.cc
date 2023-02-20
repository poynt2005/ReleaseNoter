#include <iostream>
#include <stdlib.h>
#include <filesystem>
#include <windows.h>
#include <string>
#include <regex>

int main()
{

    namespace fs = std::filesystem;
    FILE *fp = _popen("python -m compileall .", "r");

    char buf[255];
    while (fgets(buf, 255, fp) != NULL)
    {
        std::cout << buf << '\n';
    }

    _pclose(fp);

    if (fs::exists("pyBytecode"))
    {
        fs::remove_all("pyBytecode");
    }

    if (fs::exists("__pycache__"))
    {
        fs::rename("__pycache__", "pyBytecode");

        auto entries = fs::directory_iterator("pyBytecode");

        for (auto &&entry : entries)
        {
            auto file = entry.path();

            if (std::regex_search(file.filename().string(), std::regex("^ReleaseNoter")))
            {
                fs::rename(file, "pyBytecode/ReleaseNoter.pyc");
            }
        }
    }

    return 0;
}