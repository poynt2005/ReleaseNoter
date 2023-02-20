#ifndef __VS_FINDER_H__
#define __VS_FINDER_H__

#include <string>
#include <vector>
#include <map>

#define STRING_TYPE 0xe0
#define NUMBER_TYPE 0xe2
#define BOOLEAN_TYPE 0xe3
#define NULL_TYPE 0xe4

using val = struct _val
{
    int valType;
    std::string stringVal;
    double numberVal;
    bool boolVal;
};

#ifdef BUILDVSFINDERAPI
#define EXPORTVSFINERAPI __declspec(dllexport)
#else
#define EXPORTVSFINERAPI __declspec(dllimport)
#endif

EXPORTVSFINERAPI bool GetVsInfo(std::vector<std::map<std::string, val>> &);

#endif