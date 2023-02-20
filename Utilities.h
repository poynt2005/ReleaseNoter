#ifndef __UTILITIES_H__
#define __UTILITIES_H__
#include <string>
#include <vector>
namespace Utilities
{
    bool findGitRepositoryFolder(std::string &);
    void stringSplit(const std::string &, const std::string &, std::vector<std::string> &);
    void stringTrim(const std::string &, std::string &);
}

#endif