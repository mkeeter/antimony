#version 120

varying vec2 texture_coord;
uniform sampler2D depth_tex;

uniform float zmin_local;
uniform float dz_local;
uniform float zmin_global;
uniform float dz_global;

uniform vec3 color;

void main() {
    vec4 depth = texture2D(depth_tex, texture_coord);

    float fd_local = depth.r * dz_local + zmin_local;
    float fd_global = (fd_local - zmin_global) / dz_global;

    if (depth.r == 0.0f)
        fd_global = 0.0f;

    gl_FragColor = vec4(color.r * fd_global,
                        color.g * fd_global,
                        color.b * fd_global, 1.0f);
}
