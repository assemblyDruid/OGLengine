#include "logging.h"

void
Log(const LogType&& lt, const char* const&& c_str)
{
    FILE*       output_dest = stdout;
    std::string output_tag  = "";

    switch (lt)
    {
        case LogType::INFO:
            output_tag = "[ info ]";
            break;
        case LogType::WARNING:
            output_tag = "[ warning ]";
            break;
        case LogType::ERROR:
            output_tag  = "[ error ]";
            output_dest = stderr;
            break;
    }

    std::fprintf(output_dest, "%s: %s\n", output_tag.c_str(), c_str);
    std::fflush(output_dest);
}

void
Log(const LogType&& lt, const std::stringstream& ss)
{
    Log(std::move(lt), std::move(ss.str().c_str()));
}

void
Log(const LogType&& lt, const std::string& str)
{
    Log(std::move(lt), std::move(str.c_str()));
}
