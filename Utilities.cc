#include "Utilities.h"

#include <filesystem>
#include <functional>
#include <algorithm>
#include <regex>

namespace fs = std::filesystem;

namespace Utilities
{

    bool findGitRepositoryFolder(std::string &gitPath)
    {

        std::function<std::string(const fs::path &p)> findDirectory;

        findDirectory = [&findDirectory](const fs::path &p) -> std::string
        {
            for (auto &&entry : fs::directory_iterator(p))
            {
                if (fs::is_directory(entry.path()) && entry.path().filename().string() == ".git")
                {
                    return entry.path().string();
                }
            }

            if (!p.has_parent_path() || p.parent_path() == p)
            {
                return std::string("");
            }

            return findDirectory(p.parent_path());
        };

        gitPath = findDirectory(fs::current_path());

        return gitPath.length() > 0;
    }

    void stringSplit(const std::string &inString, const std::string &pattern, std::vector<std::string> &outVec)
    {
        std::regex rePattern(pattern);
        std::sregex_token_iterator regexStart(inString.begin(), inString.end(), rePattern, -1);
        std::sregex_token_iterator regexEnd;

        outVec = std::vector<std::string>(regexStart, regexEnd);
    }

    void stringTrim(const std::string &inStr, std::string &outStr)
    {
        std::regex trimRegex("^\\s+|\\s+$");

        outStr = std::regex_replace(inStr, trimRegex, "");
    }
}
