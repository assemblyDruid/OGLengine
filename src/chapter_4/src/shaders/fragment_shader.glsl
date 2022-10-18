#version 450

in vec4 varying_color;
out vec4 color;

void main()
{
    color = varying_color;
}
