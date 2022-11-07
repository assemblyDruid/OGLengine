#include "image_tools.h"

#include "logging.h"
#include "stb/stb_image.h"

#include <filesystem>
#include <sstream>

const void
ImageLoader::LoadImageToMemory(const char* const _file_path,
                               unsigned char**   _image_data,
                               int&              _image_width,
                               int&              _image_height,
                               int&              _image_channels,
                               bool&             _success)
{
    _success = true;

    if (nullptr == _file_path)
    {
        Log_e("Null pointer received for file path.");
        _success = false;
        return;
    }
    else if (nullptr == _image_data)
    {
        Log_e("Null pointer received for iamge data.");
        _success = false;
        return;
    }

    std::filesystem::path file_path(_file_path);
    if (false == std::filesystem::exists(file_path))
    {
        std::stringstream ss;
        ss << "The file " << _file_path << " does not exist.";
        Log_e(ss);
        _success = false;
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

        _success = false;
        return;
    }
    else if (nullptr == *_image_data)
    {
        Log_e("Unable to load iamge.");
        _success = false;
        return;
    }

    _image_width    = width;
    _image_height   = height;
    _image_channels = channels;

    return;
}