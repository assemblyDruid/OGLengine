#ifndef logging_h
#define logging_h

// clang-format off
#include "pch.h"
// clang-format on

#define Log_e(msg) Log::Error(msg, std::move(__FILE__), std::move(__LINE__))
#define Log_w(msg) Log::Warn(msg, std::move(__FILE__), std::move(__LINE__))
#define Log_i(msg) Log::Info(msg, std::move(__FILE__), std::move(__LINE__))

struct Log
{
    enum class LogType
    {
        ltINFO = 0,
        ltWARNING,
        ltERROR,
    };

    //
    // Error
    //
    static void
    Error(const char* const&&  c_str,
          const char* const&&  _file = "",
          const unsigned int&& _line = 0) noexcept;

    static void
    Error(const std::stringstream& ss,
          const char* const&&      _file = "",
          const unsigned int&&     _line = 0) noexcept;

    static void
    Error(const std::string&   str,
          const char* const&&  _file = "",
          const unsigned int&& _line = 0) noexcept;

    //
    // Warn
    //
    static void
    Warn(const char* const&&  c_str,
         const char* const&&  _file = "",
         const unsigned int&& _line = 0) noexcept;

    static void
    Warn(const std::stringstream& ss,
         const char* const&&      _file = "",
         const unsigned int&&     _line = 0) noexcept;

    static void
    Warn(const std::string&   str,
         const char* const&&  _file = "",
         const unsigned int&& _line = 0) noexcept;

    //
    // Info
    //
    static void
    Info(const char* const&&  c_str,
         const char* const&&  _file = "",
         const unsigned int&& _line = 0) noexcept;

    static void
    Info(const std::stringstream& ss,
         const char* const&&      _file = "",
         const unsigned int&&     _line = 0) noexcept;

    static void
    Info(const std::string&   str,
         const char* const&&  _file = "",
         const unsigned int&& _line = 0) noexcept;

  private:
    static inline void
    _Log(const LogType&&      lt,
         const char* const&&  c_str,
         const char* const&&  _file = "",
         const unsigned int&& _line = 0) noexcept;

    static inline void
    _Log(const LogType&&          lt,
         const std::stringstream& ss,
         const char* const&&      _file = "",
         const unsigned int&&     _line = 0) noexcept;

    static inline void
    _Log(const LogType&&      lt,
         const std::string&   str,
         const char* const&&  _file = "",
         const unsigned int&& _line = 0) noexcept;
};

#endif
