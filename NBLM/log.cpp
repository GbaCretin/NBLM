#include "log.hpp"
#include <cstdio>
#include <string>

void stdLog::debug(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    std::string prefix("[DEBUG] ");
    std::string formatStr(format);
    std::string nChar("\n");

    vprintf((prefix + formatStr + nChar).c_str(), argptr);
    va_end(argptr);
}

void stdLog::info(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    std::string prefix("[INFO] ");
    std::string formatStr(format);
    std::string nChar("\n");

    vprintf((prefix + formatStr + nChar).c_str(), argptr);
    va_end(argptr);
}

void stdLog::warning(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    std::string prefix("[WARNING] ");
    std::string formatStr(format);
    std::string nChar("\n");

    vfprintf(stderr, (prefix + formatStr + nChar).c_str(), argptr);
    va_end(argptr);
}

void stdLog::critical(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    std::string prefix("[CRITICAL] ");
    std::string formatStr(format);
    std::string nChar("\n");

    vfprintf(stderr, (prefix + formatStr + nChar).c_str(), argptr);
    va_end(argptr);
}

void stdLog::fatal(const char* format, ...)
{
    va_list argptr;
    va_start(argptr, format);
    std::string prefix("[FATAL] ");
    std::string formatStr(format);
    std::string nChar("\n");

    vfprintf(stderr, (prefix + formatStr + nChar).c_str(), argptr);
    va_end(argptr);
    exit(1);
}
