#include <stdlib.h>
#include <stdio.h>

#include "render/render.h"
#include "render/shader.h"

// Global variables shared for the renderer
GLFWwindow* window;

int gl_init(void)
{
    printf("gl_init called\n");
    // Initialize the library
    if (!glfwInit())    return -1;

    glfwWindowHint(GLFW_SAMPLES, 8);    // multisampling!
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(
            640, 480, "context", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make sure that we made a 3.3+ context.
    glfwMakeContextCurrent(window);
    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    if (major < 3 || (major == 3 && minor < 3))
    {
        glfwTerminate();
        return -1;
    }
    printf("OpenGL initialized!\n");

    shader_compile_frag("/Users/mkeeter/code/antimony/fab/c/render/eval.frag");
    return 0;
}
