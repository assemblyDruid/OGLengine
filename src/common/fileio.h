#ifndef fileio_h
#define fileio_h

// clang-format off
#include "pch.h"
// clang-format on

void
FileExistsOrEmpty(const char* const _file_path_in, bool& _file_exists_out, bool& _file_empty_out);

void
LoadFileToStdString(bool&             _success_out,
                    const char* const _file_path_in,
                    size_t&           _file_size_out,
                    std::string&      _file_data_out);

#endif // fileio_h