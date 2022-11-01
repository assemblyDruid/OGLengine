#define _CRT_SECURE_NO_WARNINGS
#include "logging.h"

#include <chrono>
#include <vector>

//
// Error
//
const void
Log::Error(const char* const&& c_str, const char* const&& _file, const unsigned int&& _line)
{
    _Log(std::move(LogType::ERROR), std::move(c_str), std::move(_file), std::move(_line));
}

const void
Log::Error(const std::stringstream& ss, const char* const&& _file, const unsigned int&& _line)
{
    _Log(std::move(LogType::ERROR), std::move(ss), std::move(_file), std::move(_line));
}

const void
Log::Error(const std::string& str, const char* const&& _file, const unsigned int&& _line)
{
    _Log(std::move(LogType::ERROR), std::move(str), std::move(_file), std::move(_line));
}

//
// Warn
//
const void
Log::Warn(const char* const&& c_str, const char* const&& _file, const unsigned int&& _line)
{
    _Log(std::move(LogType::WARNING), std::move(c_str), std::move(_file), std::move(_line));
}

const void
Log::Warn(const std::stringstream& ss, const char* const&& _file, const unsigned int&& _line)
{
    _Log(std::move(LogType::WARNING), std::move(ss), std::move(_file), std::move(_line));
}

const void
Log::Warn(const std::string& str, const char* const&& _file, const unsigned int&& _line)
{
    _Log(std::move(LogType::WARNING), std::move(str), std::move(_file), std::move(_line));
}

//
// Info
//
const void
Log::Info(const char* const&& c_str, const char* const&& _file, const unsigned int&& _line)
{
    _Log(std::move(LogType::INFO), std::move(c_str), std::move(_file), std::move(_line));
}

const void
Log::Info(const std::stringstream& ss, const char* const&& _file, const unsigned int&& _line)
{
    _Log(std::move(LogType::INFO), std::move(ss), std::move(_file), std::move(_line));
}

const void
Log::Info(const std::string& str, const char* const&& _file, const unsigned int&& _line)
{
    _Log(std::move(LogType::INFO), std::move(str), std::move(_file), std::move(_line));
}

const inline void
Log::_Log(const LogType&&      lt,
          const char* const&&  c_str,
          const char* const&&  _file,
          const unsigned int&& _line)
{
    time_t time_point = time(NULL);

    FILE*       output_dest = stdout;
    std::string output_tag  = "";
    std::string spacing     = "\n";

    switch (lt)
    {
        case LogType::INFO:
            output_tag = "[ info ]: ";
            spacing += "          ";
            break;
        case LogType::WARNING:
            output_tag = "[ warning ]: ";
            spacing += "             ";
            break;
        case LogType::ERROR:
            output_tag = "[ error ]: ";
            spacing += "           ";
            output_dest = stderr;
            break;
    }

    std::stringstream message;
    message << output_tag << c_str << std::endl;
    if (std::string(_file).size() > 0)
    {
        std::stringstream file_line_tag;
        file_line_tag << spacing << _file << ':' << _line;
        message << file_line_tag.str().c_str();
    }
    message << spacing << std::ctime(&time_point) << std::endl;
    std::fprintf(output_dest, "%s\n", message.str().c_str());
    std::fflush(output_dest);
}

const inline void
Log::_Log(const LogType&&          lt,
          const std::stringstream& ss,
          const char* const&&      _file,
          const unsigned int&&     _line)
{
    _Log(std::move(lt), std::move(ss.str().c_str()), std::move(_file), std::move(_line));
}

const inline void
Log::_Log(const LogType&&      lt,
          const std::string&   str,
          const char* const&&  _file,
          const unsigned int&& _line)
{
    _Log(std::move(lt), std::move(str.c_str()), std::move(_file), std::move(_line));
}

#undef _CRT_SECURE_NO_WARNINGS