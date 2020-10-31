#ifndef DEBUG_H
#define DEBUG_H

#include <cstdarg>

namespace stdLog
{
    void debug(const char* format, ...);
    void info(const char* format, ...);
    void warning(const char* format, ...);
    void critical(const char* format, ...);
    void fatal(const char* format, ...);
}

#endif // DEBUG_H
