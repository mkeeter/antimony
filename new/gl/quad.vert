#version 120
attribute vec2 vertex_position;

uniform vec2 offset;
uniform float width;
uniform float height;

varying vec2 texture_coord;

void main() {
    gl_Position = vec4(vertex_position.x * width  + offset.x,
                       vertex_position.y * height + offset.y,
                       0.0f, 1.0f);
    texture_coord = vec2(0.5f + vertex_position.x/2.0f,
                         0.5f - vertex_position.y/2.0f);
}
