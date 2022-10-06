#version 450

uniform float t;
out     float offset;

//
// Retuns a transformation matrix.
//
mat4 GetTranslateMatrix(float x, float y, float z)
{
    mat4 transform = mat4(1.0, 0.0, 0.0, 0.0,
                          0.0, 1.0, 0.0, 0.0,
                          0.0, 1.0, 1.0, 0.0,
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


//
// Returns a scale matrix.
//
mat4 GetScaleMatrix(float x, float y, float z)
{
    mat4 scale = mat4(  x, 0.0, 0.0, 0.0,
                      0.0,   y, 0.0, 0.0,
                      0.0, 0.0,   z, 0.0,
                      0.0, 0.0, 0.0, 1.0);

    return scale;
}


void main()
{
    mat4 M = GetRotateZMatrix(1.57 + t) * GetRotateXMatrix(3.14 + t) *
             GetRotateYMatrix(3.14 + t) * GetTranslateMatrix(t, t, t) *
             GetScaleMatrix(1 + t, 1 + t, 1 + t);

    if (0 == gl_VertexID)
    {
        gl_Position = M * vec4(0.25 + t, -0.25, 0.0, 1.0);
    }
    else if (1 == gl_VertexID)
    {
        gl_Position = M * vec4(-0.25 + t, -0.25, 0.0, 1.0);
    }
    else
    {
        gl_Position = M * vec4(0.0 + t, 0.25, 0.0, 1.0);
    }

    offset = t;
}
