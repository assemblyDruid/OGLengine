#version 450

layout (location = 0) in vec3 position;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec4 color;

void main()
{
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
}
