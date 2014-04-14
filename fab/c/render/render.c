#include <stdlib.h>
#include <stdio.h>

#include "render/render.h"
#include "render/shader.h"
#include "render/command.h"
#include "render/tape.h"

// Global variables shared for the renderer
GLFWwindow* window;

// Shader programs for eval and blit stages
GLuint eval_program;
GLuint blit_program;

int gl_init(char* shader_dir)
{
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

    GLuint  tex = shader_compile_vert(shader_dir, "texture.vert");
    GLuint eval = shader_compile_frag(shader_dir, "eval.frag");
    GLuint blit = shader_compile_frag(shader_dir, "blit.frag");

    eval_program = shader_make_program(tex, eval);
    blit_program = shader_make_program(tex, blit);
    return 0;
}

void render_eval(const RenderCommand* const command,
                 const RenderTape* const tape)
{
    const GLuint program = eval_program;
    glUseProgram(program);

    // Bind tape texture to 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D, tape->instructions);
    glUniform1i(glGetUniformLocation(program, "tape"), 0);

    // Bind atlas texture to 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, command->atlas);
    glUniform1i(glGetUniformLocation(program, "atlas"), 1);

    // Bind xyz data to 2
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_1D, command->xyz);
    glUniform1i(glGetUniformLocation(program, "xyz"), 2);

    // Load relevant uniforms
    glUniform1i(glGetUniformLocation(program, "block_size"),
                command->block_size);
    glUniform1i(glGetUniformLocation(program, "block_count"),
                command->block_count);

    // Target the swap texture with the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, command->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, command->swap, 0);

    // Prepare the viewport
    glViewport(0, 0, command->block_size + 1, tape->node_count + 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Load triangles that draw a flat rectangle from -1, -1, to 1, 1
    glBindBuffer(GL_ARRAY_BUFFER, command->rect);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);

    // Draw the full rectangle into the FBO
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render_blit(const RenderCommand* const command,
                 const RenderTape* const tape,
                 const GLint start_slot)
{
    const GLuint program = blit_program;
    glUseProgram(program);

    // Bind tape texture to 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, command->swap);
    glUniform1i(glGetUniformLocation(program, "values"), 0);

    // Load relevant uniforms
    glUniform1i(glGetUniformLocation(program, "block_size"),
                command->block_size);
    glUniform1i(glGetUniformLocation(program, "block_count"),
                command->block_count);
    glUniform1i(glGetUniformLocation(program, "start_slot"),
                start_slot);
    glUniform1i(glGetUniformLocation(program, "slot_count"),
                tape->node_count);

    // Target the swap texture with the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, command->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, command->atlas, 0);

    // Prepare the viewport (contains the whole texture atlas)
    glViewport(0, 0, command->block_size * command->block_count + 1,
               command->node_count + 1);
    if (start_slot == 0)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Load triangles that draw a flat rectangle from -1, -1, to 1, 1
    glBindBuffer(GL_ARRAY_BUFFER, command->rect);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);

    // Draw the full rectangle into the FBO
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render_command(RenderCommand* command, float* xyz)
{
    glfwMakeContextCurrent(window);

    // Copy xyz data to the xyz texture.
    glBindTexture(GL_TEXTURE_1D, command->xyz);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB32F, command->block_size,
                 0, GL_RGB, GL_FLOAT, xyz);

    glBindVertexArray(command->vao);

    RenderTape* tape = command->tape;
    GLint current_slot = 0;
    while (tape)
    {
        render_eval(command, tape);
        render_blit(command, tape, current_slot);
        current_slot += tape->node_count;
        tape = tape->next;
    }

    // Switch back to the default framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
