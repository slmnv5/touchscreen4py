#ifndef UTILS_H
#define UTILS_H

#include "pch.hpp"

//==================== utility functions ===================================

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

std::vector<std::string> split_string(const char *s, char chrDelim)
{
    std::stringstream strSream(s);
    std::string segment;
    std::vector<std::string> segVector;

    while (std::getline(strSream, segment, chrDelim))
    {
        segVector.push_back(segment);
    }
    return segVector;
}

#endif