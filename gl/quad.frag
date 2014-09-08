#version 120

varying vec2 texture_coord;
uniform sampler2D depth_tex;
uniform sampler2D shaded_tex;

uniform float zmin_local;
uniform float dz_local;
uniform float zmin_global;
uniform float dz_global;

vec4 shade(vec4 norm)
{
    vec3 light = vec3(0.99, 0.96, 0.89);
    vec3 dark = vec3(0.20, 0.25, 0.3);

    float a = dot(norm.xyz, vec3(0.57, 0.57, 0.57));
    float b = dot(norm.xyz, vec3(-0.57, -0.57, 0.57));

    return vec4((a*light + (1-a)*dark)*0.65 +
                (b*light + (1-b)*dark)*0.65, 1);
}

void main() {
    vec4 depth = texture2D(depth_tex, texture_coord);

    if (depth.r == 0.0f)
    {
        discard;
    }

    gl_FragColor = shade(texture2D(shaded_tex, texture_coord));

    // Due to the way that Qt renders a GraphicsScene
    // (with OpenGL acceleration), we're limited to the
    // depth range of (0.5, 1].  In practice, we'll upse
    // [0.6, 0.9]
    float fd_local = depth.r * dz_local + zmin_local;
    float fd_global = (fd_local - zmin_global) / dz_global;
    gl_FragDepth = 0.9 - fd_global*0.3;
}
