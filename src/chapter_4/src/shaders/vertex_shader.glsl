#version 450

layout (location = 0) in vec3 position;

uniform mat4 proj_matrix;
uniform mat4 mv_matrix;

out vec4 color;
out vec4 varying_color;

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
    gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
    varying_color = vec4(position, 1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}
