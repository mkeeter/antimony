#version 120
attribute vec2 vertex_position;

uniform vec2 offset;
uniform float width;
uniform float height;

void main() {
    gl_Position = vec4(vertex_position.x * width  + offset.x,
                       vertex_position.y * height + offset.y,
                       0.0, 1.0);
}
