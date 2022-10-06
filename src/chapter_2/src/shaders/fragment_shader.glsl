#version 450

in  float offset;
out vec4  color;

void main()
{
    color = vec4((1.0 - offset), 0.0, 0.0, 1.0);
}
