#version 460

layout(binding = 0) uniform sampler2D us2d; // [ cfarvin::TODO ] Move to vertex shader?

in vec2  texture_coordinates;
out vec4 color;

void main()
{
    color = texture(us2d, texture_coordinates);
}
