#ifndef image_tools_h
#define image_tools_h

void
LoadImageToMemory(bool&             _success_out,
                  const char* const _file_path,
                  unsigned char**   _image_data,
                  int&              _image_width,
                  int&              _image_height,
                  int&              _image_channels);

#endif