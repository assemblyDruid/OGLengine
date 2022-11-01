#ifndef image_tools_h
#define image_tools_h

#include "stb/stb_image.h"

#include <cstdint>
#include <string>
#include <vector>

struct ImageLoader
{
    static const void
    LoadImageToMemory(const char* const _file_path,
                      unsigned char**   _image_data,
                      int&              _image_width,
                      int&              _image_height,
                      int&              _image_channels,
                      bool&             _success);
};

#endif