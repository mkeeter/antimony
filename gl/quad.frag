#version 120

varying vec2 texture_coord;
uniform sampler2D depth_tex;

uniform float zmin_local;
uniform float dz_local;
uniform float zmin_global;
uniform float dz_global;

void main() {
    vec4 color = texture2D(depth_tex, texture_coord);

    if (color.r == 0.0f)
    {
        discard;
    }

    gl_FragColor = vec4(color.r, color.g, color.b, 1.0);

    // Due to the way that Qt renders a GraphicsScene
    // (with OpenGL acceleration), we're limited to the
    // depth range of (0.5, 1].  In practice, we'll upse
    // [0.6, 0.9]
    float fd_local = color.r * dz_local + zmin_local;
    float fd_global = (fd_local - zmin_global) / dz_global;
    gl_FragDepth = 0.9 - fd_global*0.3;
}
