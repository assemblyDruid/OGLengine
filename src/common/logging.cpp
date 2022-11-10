// clang-format off
#include "pch.h"
// clang-format on

#include "logging.h"

//
// Error
//
void
Log::Error(const char* const&&  c_str,
           const char* const&&  _file,
           const unsigned int&& _line) noexcept
{
    _Log(std::move(LogType::ltERROR), std::move(c_str), std::move(_file), std::move(_line));
}

void
Log::Error(const std::stringstream& ss,
           const char* const&&      _file,
           const unsigned int&&     _line) noexcept
{
    _Log(std::move(LogType::ltERROR), std::move(ss), std::move(_file), std::move(_line));
}

void
Log::Error(const std::string& str, const char* const&& _file, const unsigned int&& _line) noexcept
{
    _Log(std::move(LogType::ltERROR), std::move(str), std::move(_file), std::move(_line));
}

//
// Warn
//
void
Log::Warn(const char* const&& c_str, const char* const&& _file, const unsigned int&& _line) noexcept
{
    _Log(std::move(LogType::ltWARNING), std::move(c_str), std::move(_file), std::move(_line));
}

void
Log::Warn(const std::stringstream& ss,
          const char* const&&      _file,
          const unsigned int&&     _line) noexcept
{
    _Log(std::move(LogType::ltWARNING), std::move(ss), std::move(_file), std::move(_line));
}

void
Log::Warn(const std::string& str, const char* const&& _file, const unsigned int&& _line) noexcept
{
    _Log(std::move(LogType::ltWARNING), std::move(str), std::move(_file), std::move(_line));
}

//
// Info
//
void
Log::Info(const char* const&& c_str, const char* const&& _file, const unsigned int&& _line) noexcept
{
    _Log(std::move(LogType::ltINFO), std::move(c_str), std::move(_file), std::move(_line));
}

void
Log::Info(const std::stringstream& ss,
          const char* const&&      _file,
          const unsigned int&&     _line) noexcept
{
    _Log(std::move(LogType::ltINFO), std::move(ss), std::move(_file), std::move(_line));
}

void
Log::Info(const std::string& str, const char* const&& _file, const unsigned int&& _line) noexcept
{
    _Log(std::move(LogType::ltINFO), std::move(str), std::move(_file), std::move(_line));
}

void
Log::_Log(const LogType&&      lt,
          const char* const&&  c_str,
          const char* const&&  _file,
          const unsigned int&& _line) noexcept
{
    time_t time_point = time(NULL);

    FILE*             output_dest = stdout;
    std::string       output_tag  = "";
    const std::string spacing     = "\n   > ";

    switch (lt)
    {
        case LogType::ltINFO:
            output_tag = "[ info ]: ";
            break;
        case LogType::ltWARNING:
            output_tag = "[ warning ]: ";
            break;
        case LogType::ltERROR:
            output_tag  = "[ error ]: ";
            output_dest = stderr;
            break;
    }

    std::stringstream message;
    message << output_tag << c_str;
    if (std::string(_file).size() > 0)
    {
        std::stringstream file_line_tag;
        file_line_tag << spacing << _file << ':' << _line;
        message << file_line_tag.str().c_str();
    }
    message << spacing << std::ctime(&time_point) << std::endl;
    std::fprintf(output_dest, "%s", message.str().c_str());
    std::fflush(output_dest);
}

void
Log::_Log(const LogType&&          lt,
          const std::stringstream& ss,
          const char* const&&      _file,
          const unsigned int&&     _line) noexcept
{
    _Log(std::move(lt), std::move(ss.str().c_str()), std::move(_file), std::move(_line));
}

void
Log::_Log(const LogType&&      lt,
          const std::string&   str,
          const char* const&&  _file,
          const unsigned int&& _line) noexcept
{
    _Log(std::move(lt), std::move(str.c_str()), std::move(_file), std::move(_line));
}
