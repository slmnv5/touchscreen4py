#ifndef UTILS_H
#define UTILS_H

#include "pch.hpp"

std::string exec_command(const std::string &cmd)
{
    char buffer[128];
    std::string result = "";
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("popen() failed!");
    try
    {
        while (fgets(buffer, sizeof buffer, pipe) != NULL)
        {
            result += buffer;
        }
    }
    catch (std::exception &e)
    {
        pclose(pipe);
        throw e;
    }
    pclose(pipe);
    return result;
}

std::vector<std::string> splitString(const char *s, char chrDelim)
{
    std::stringstream strSream(s);
    std::string el;
    std::vector<std::string> tokens;

    while (std::getline(strSream, el, chrDelim))
    {
        tokens.push_back(el);
    }
    return tokens;
}

std::vector<std::string> splitString(const std::string &s, const std::string &strDelim)
{
    std::vector<std::string> tokens;
    uint start = 0U;
    auto stop = s.find(strDelim);
    while (stop != std::string::npos)
    {
        tokens.push_back(s.substr(start, stop - start));
        start = stop + strDelim.length();
        stop = s.find(strDelim, start);
    }
    tokens.push_back(s.substr(start, stop));
    return tokens;
}

#endif