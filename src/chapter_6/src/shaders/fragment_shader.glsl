#version 460

layout(binding = 0) uniform sampler2D us2d;

in vec2  texture_coordinates;
in vec3  position;
out vec4 color;

void main() {
    color = texture(us2d, texture_coordinates);
    // color = vec4(position.x, position.y, 1.0, 1.0f);
}
