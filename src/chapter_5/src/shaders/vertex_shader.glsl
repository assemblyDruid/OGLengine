#version 460

layout (location = 0) in vec3 _position;
layout (location = 1) in vec2 _texture_coordinates;

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;

out vec2 texture_coordinates;

//
// Retuns a transformation matrix.
//
mat4 GetTranslateMatrix(float x, float y, float z)
{
    mat4 transform = mat4(1.0, 0.0, 0.0, 0.0,
                          0.0, 1.0, 0.0, 0.0,
                          0.0, 0.0, 1.0, 0.0,
                            x,   y,   z, 1.0);
    return transform;
}


//
// Returns a matrix that performs a rotation
// about the x-axis (in radians).
//
mat4 GetRotateXMatrix(float rad)
{
    mat4 rotx = mat4(1.0,      0.0,       0.0,  0.0,
                     0.0, cos(rad), -sin(rad),  0.0,
                     0.0, sin(rad),  cos(rad),  0.0,
                     0.0,      0.0,       0.0,  1.0);

    return rotx;
}


//
// Returns a matrix that performs a rotation
// about the y-axis (in radians).
//
mat4 GetRotateYMatrix(float rad)
{
    mat4 roty = mat4(cos(rad),  0.0,  sin(rad),  0.0,
                          0.0,  1.0,  0.0,       0.0,
                    -sin(rad),  0.0,  cos(rad),  0.0,
                          0.0,  0.0,  0.0,       1.0);


    return roty;
}


//
// Returns a matrix that performs a rotation
// about the z-axis (in radians).
//
mat4 GetRotateZMatrix(float rad)
{
    mat4 rotz = mat4(cos(rad), -sin(rad),  0.0,  0.0,
                     sin(rad),  cos(rad),  0.0,  0.0,
                          0.0,       0.0,  1.0,  0.0,
                          0.0,       0.0,  0.0,  1.0);


    return rotz;
}

void main()
{
    gl_Position = proj_matrix * mv_matrix * vec4(_position, 1.0);
    texture_coordinates = _texture_coordinates;
}
