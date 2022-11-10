// clang-format off
#include "pch.h"
// clang-format on

#include "fileio.h"

#include "logging.h"

void
FileExistsOrEmpty(const char* const _file_path_in, bool& _file_exists_out, bool& _file_empty_out)
{
    std::filesystem::exists(_file_path_in) ? _file_exists_out = true : _file_exists_out = false;

    if (_file_exists_out)
    {
        std::filesystem::is_empty(_file_path_in) ? _file_empty_out = true : _file_empty_out = false;
    }
    else
    {
        _file_empty_out = true;
    }
}

void
LoadFileToStdString(bool&             _success_out,
                    const char* const _file_path_in,
                    size_t&           _file_size_out,
                    std::string&      _file_data_out)
{
    _success_out     = true;
    bool file_exists = false;
    bool file_empty  = true;
    FileExistsOrEmpty(_file_path_in, file_exists, file_empty);
    if ((false == file_exists) || (true == file_empty))
    {
        std::stringstream ss;
        ss << "File does not exist or is empty: " << _file_path_in;
        Log_e(ss);
        _success_out = false;
    }

    if (false == _success_out) return;

    const std::filesystem::path file_path(_file_path_in);
    std::ifstream               file_stream(file_path);

    _file_size_out = std::filesystem::file_size(_file_path_in);
    _file_data_out = std::string(_file_size_out, '\0');

    file_stream.read(_file_data_out.data(), _file_size_out);

    return;
}