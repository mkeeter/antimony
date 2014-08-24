#version 120

varying vec2 texture_coord;
uniform sampler2D depth_tex;

void main() {
    vec4 color = texture2D(depth_tex, texture_coord);
    gl_FragColor = vec4(color.r, color.g, color.b, 1.0);
}
