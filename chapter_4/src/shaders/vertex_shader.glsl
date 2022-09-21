#version 450

layout (location = 0) in vec3 position;

<<<<<<< HEAD
=======
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

>>>>>>> bb0c7bc (Moved repeated code to modules in common folder)
void main()
{
    gl_Position = vec4(position, 1.0);
}
