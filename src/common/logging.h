#ifndef logging_h
#define logging_h

#include <sstream>
#include <string>

#define Log_e(msg) Log::Error(msg, std::move(__FILE__), std::move(__LINE__))
#define Log_w(msg) Log::Warn(msg, std::move(__FILE__), std::move(__LINE__))
#define Log_i(msg) Log::Info(msg, std::move(__FILE__), std::move(__LINE__))

struct Log
{
    enum class LogType
    {
        INFO = 0,
        WARNING,
        ERROR,
    };

    //
    // Error
    //
    static const void
    Error(const char* const&&  c_str,
          const char* const&&  _file = "",
          const unsigned int&& _line = 0);

    static const void
    Error(const std::stringstream& ss,
          const char* const&&      _file = "",
          const unsigned int&&     _line = 0);

    static const void
    Error(const std::string& str, const char* const&& _file = "", const unsigned int&& _line = 0);

    //
    // Warn
    //
    static const void
    Warn(const char* const&& c_str, const char* const&& _file = "", const unsigned int&& _line = 0);

    static const void
    Warn(const std::stringstream& ss,
         const char* const&&      _file = "",
         const unsigned int&&     _line = 0);

    static const void
    Warn(const std::string& str, const char* const&& _file = "", const unsigned int&& _line = 0);

    //
    // Info
    //
    static const void
    Info(const char* const&& c_str, const char* const&& _file = "", const unsigned int&& _line = 0);

    static const void
    Info(const std::stringstream& ss,
         const char* const&&      _file = "",
         const unsigned int&&     _line = 0);

    static const void
    Info(const std::string& str, const char* const&& _file = "", const unsigned int&& _line = 0);

  private:
    static const void
    _Log(const LogType&&      lt,
         const char* const&&  c_str,
         const char* const&&  _file = "",
         const unsigned int&& _line = 0);

    static const void
    _Log(const LogType&&          lt,
         const std::stringstream& ss,
         const char* const&&      _file = "",
         const unsigned int&&     _line = 0);

    static const void
    _Log(const LogType&&      lt,
         const std::string&   str,
         const char* const&&  _file = "",
         const unsigned int&& _line = 0);
};

#endif
