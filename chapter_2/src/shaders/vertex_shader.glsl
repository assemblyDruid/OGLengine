#version 450

uniform float t;
out     float offset;

void main()
{
    if (0 == gl_VertexID)
    {
        gl_Position = vec4(0.25 + t, -0.25, 0.0, 1.0);
    }
    else if (1 == gl_VertexID)
    {
        gl_Position = vec4(-0.25 + t, -0.25, 0.0, 1.0);
    }
    else
    {
        gl_Position = vec4(0.0 + t, 0.25, 0.0, 1.0);
    }

    offset = t;
}
