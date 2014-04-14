#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "render/shader.h"

GLuint shader_make_program(const GLuint vert, const GLuint frag)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint info_log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

        GLchar* info_log = malloc(sizeof(GLchar) * info_log_length);
        glGetProgramInfoLog(program, info_log_length, NULL, info_log);

        printf("Linker failure: %s\n", info_log);

        free(info_log);
    }

    return program;
}

////////////////////////////////////////////////////////////////////////////////

GLuint shader_compile_vert(char* folder, char* filename)
{
    return shader_compile(folder, filename, GL_VERTEX_SHADER);
}

////////////////////////////////////////////////////////////////////////////////

GLuint shader_compile_frag(char* folder, char* filename)
{
    return shader_compile(folder, filename, GL_FRAGMENT_SHADER);
}

////////////////////////////////////////////////////////////////////////////////

GLuint shader_compile(char* folder, char* filename, GLenum type)
{
    // Concatenate folder and file name
    size_t len = strlen(folder) + strlen(filename);
    char* const path = calloc(sizeof(char), len);
    strcat(path, folder);
    strcat(path, filename);

    // Open the file and check for existance
    FILE* file = fopen(path, "rb");
    if (!file) {
        printf("No such file %s\n", path);
        return 0;
    }

    // Figure out the size of the file
    fseek(file, 0, SEEK_END);
    GLint end = ftell(file);
    rewind(file);

    // Load the shader's text into a character array
    char* const txt = malloc(sizeof(GLchar) * end);
    fread(txt, end, 1, file);
    fclose(file);

    // Attempt to compile the shader
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar*const*)&txt, &end);
    glCompileShader(shader);

    // Check the compile status.
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint info_log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

        GLchar* info_log = malloc(sizeof(GLchar) * info_log_length);
        glGetShaderInfoLog(shader, info_log_length, NULL, info_log);

        printf("Compile failure in %s shader (%s):\n%s\n",
                type == GL_VERTEX_SHADER ? "vertex" : "fragment",
                filename, info_log);
        free(info_log);
    }

    return shader;
}
