#ifndef RENDER_H
#define RENDER_H

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

/** @brief Creates an OpenGL context
    @param shader_dir Directory where shaders are found.
    @returns -1 in case of failure, 0 in case of success.
*/
int gl_init(char* shader_dir);

/** @brief Sets texture defaults
    @param tex_enum GL_TEXTURE_1D or GL_TEXTURE_2D
*/
void gl_tex_defaults(GLenum tex_enum);

/** @brief OpenGL window (for context)
*/
extern GLFWwindow* window;

#endif
