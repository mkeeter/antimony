#version 330

layout(location=0) in vec2 vertex_position;

// Expects to get a rectangle from -1, -1 to 1, 1
void main()
{
    gl_Position = vec4(vertex_position, 0.0f, 1.0f);
}
