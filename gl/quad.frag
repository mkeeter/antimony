#version 120

varying vec2 texture_coord;
uniform sampler2D depth_tex;

uniform float zmin_local;
uniform float dz_local;
uniform float zmin_global;
uniform float dz_global;

void main() {
    vec4 color = texture2D(depth_tex, texture_coord);
    gl_FragColor = vec4(color.r, color.g, color.b, 1.0);

    // Due to the way that Qt renders a GraphicsScene
    // (with OpenGL acceleration), we're limited to the
    // depth range of (0.5, 1].  In practice, we'll upse
    // [0.6, 0.9]
    gl_FragDepth = 0.9 - color.r*0.3;
}
