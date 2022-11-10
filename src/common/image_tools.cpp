// clang-format off
#include "pch.h"
// clang-format on

#include "image_tools.h"

#include "fileio.h"
#include "logging.h"

void
LoadImageToMemory(bool&             _success_out,
                  const char* const _file_path,
                  unsigned char**   _image_data,
                  int&              _image_width,
                  int&              _image_height,
                  int&              _image_channels)
{
    _success_out = true;

    if (nullptr == _file_path)
    {
        Log_e("Null pointer received for file path.");
        _success_out = false;
        return;
    }
    else if (nullptr == _image_data)
    {
        Log_e("Null pointer received for iamge data.");
        _success_out = false;
        return;
    }

    bool file_exists;
    bool file_empty;
    FileExistsOrEmpty(_file_path, file_exists, file_empty);
    if ((false == file_exists) || (true == file_empty))
    {
        std::stringstream ss;
        ss << "The file " << _file_path << " does not exist or is empty.";
        Log_e(ss);
        _success_out = false;
        return;
    }

    _image_width = _image_height = _image_channels = 0;
    int width, height, channels = 0;
    stbi_set_flip_vertically_on_load(true);
    *_image_data = stbi_load(_file_path, &width, &height, &channels, STBI_rgb_alpha);

    if (0 >= width || 0 >= height || 0 >= channels)
    {
        Log_e("Loaded image has invalid dimensions.");

        if (nullptr != *_image_data)
        {
            delete *_image_data;
        }

        _success_out = false;
        return;
    }
    else if (nullptr == *_image_data)
    {
        Log_e("Unable to load iamge.");
        _success_out = false;
        return;
    }

    _image_width    = width;
    _image_height   = height;
    _image_channels = channels;

    return;
}