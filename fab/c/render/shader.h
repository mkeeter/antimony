#ifndef SHADER_H
#define SHADER_H

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

GLuint shader_make_program(const GLuint vert, const GLuint frag);
GLuint shader_compile_vert(char* folder, char* filename);
GLuint shader_compile_frag(char* folder, char* filename);
GLuint shader_compile(char* folder, char* filename, GLenum type);

#endif
