#ifndef RENDER_H
#define RENDER_H

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

/** @brief Creates an OpenGL context
    @returns -1 in case of failure, 0 in case of success.
*/
int gl_init(void);

/** @brief OpenGL window (for context)
*/
extern GLFWwindow* window;

#endif
