#ifndef logging_h
#define logging_h

#include <sstream>
#include <string>

enum class LogType
{
    INFO = 0,
    WARNING,
    ERROR,
};

void
Log(const LogType&& lt, const char* const&& c_str);

void
Log(const LogType&& lt, const std::stringstream& ss);

void
Log(const LogType&& lt, const std::string& str);

#endif
