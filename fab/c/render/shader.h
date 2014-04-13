#ifndef SHADER_H
#define SHADER_H

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

GLuint shader_compile_vert(char* filename);
GLuint shader_compile_frag(char* filename);
GLuint shader_compile(char* filename, GLenum type);

#endif
