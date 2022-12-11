#ifndef UTILS_H
#define UTILS_H

#include "pch.hpp"

//==================== utility functions ===================================

std::string find_kbd_event()
{
    const char *cmd = "grep -E 'Handlers|EV=' /proc/bus/input/devices | "
                      "grep -B1 'EV=120013' | grep -Eo 'event[0-9]+' | grep -Eo '[0-9]+' | tr -d '\n'";

    FILE *pipe = popen(cmd, "r");
    char buffer[128];
    std::string result = "";
    while (!feof(pipe))
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    pclose(pipe);
    return result;
}

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

std::vector<std::string> split_string(const std::string &s, const std::string &delimiter)
{
    std::vector<std::string> tokens;
    auto start = 0U;
    auto stop = s.find(delimiter);
    while (stop != std::string::npos)
    {
        tokens.push_back(s.substr(start, stop - start));
        start = stop + delimiter.length();
        stop = s.find(delimiter, start);
    }
    tokens.push_back(s.substr(start, stop));
    return tokens;
}

std::string word_at_position(const std::string &s, int pos, char left_space, char right_space)
{
    if (pos < 0 or pos >= s.length())
    {
        return "";
    }
    int start, stop;
    start = stop = pos;
    while (s.at(start) != left_space and start-- > 0)
        ;
    if (start < 0)
        return "";
    while (s.at(stop) != right_space and stop++ < s.length())
        ;
    if (stop >= s.length())
        return "";

    auto word = s.substr(start, stop - start + 1);
    cout << "=======" << start << "=====" << stop << "====" << word << "=====\n";
    return word;
}

#endif