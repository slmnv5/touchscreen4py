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

std::string find_touchscr_event()
{
    const char *cmd = "grep -E 'Handlers|EV=' /proc/bus/input/devices | "
                      "grep -B1 'EV=b' | grep -Eo 'event[0-9]+' | grep -Eo '[0-9]+' | tr -d '\n'";

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

#endif