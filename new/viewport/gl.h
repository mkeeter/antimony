#pragma once

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class ViewportGL
{
public:
    /*
     *  Returns the shaders for shaded and heightmap rendering, respectively
     */
    QOpenGLShaderProgram* getShadedShader();
    QOpenGLShaderProgram* getHeightmapShader();

    /*
     *  Returns the VBO for drawing textured quads
     */
    QOpenGLBuffer* getQuadVertices();

protected:
    /*
     *  Initialize the shader and VBO
     */
    void initialize();

    bool initialized=false;

    QOpenGLShaderProgram height_shader;
    QOpenGLShaderProgram shaded_shader;
    QOpenGLBuffer quad_vertices;
};
