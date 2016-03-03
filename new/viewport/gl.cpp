#include <QOpenGLWidget>

#include "viewport/gl.h"
#include "viewport/view.h"

QOpenGLBuffer* ViewportGL::getQuadVertices()
{
    if (!initialized)
        initialize();
    return &quad_vertices;
}

QOpenGLShaderProgram* ViewportGL::getShadedShader()
{
    if (!initialized)
        initialize();
    return &shaded_shader;
}

QOpenGLShaderProgram* ViewportGL::getHeightmapShader()
{
    if (!initialized)
        initialize();
    return &height_shader;
}

void ViewportGL::initialize()
{
    float vbuf[] = {
         -1, -1,
         -1,  1,
          1, -1,
          1,  1};
    quad_vertices.create();
    quad_vertices.bind();
    quad_vertices.allocate(vbuf, sizeof(vbuf));
    quad_vertices.release();

    shaded_shader.addShaderFromSourceFile(
            QOpenGLShader::Vertex, ":/gl/quad.vert");
    shaded_shader.addShaderFromSourceFile(
            QOpenGLShader::Fragment, ":/gl/shaded.frag");
    shaded_shader.link();

    height_shader.addShaderFromSourceFile(
            QOpenGLShader::Vertex, ":/gl/quad.vert");
    height_shader.addShaderFromSourceFile(
            QOpenGLShader::Fragment, ":/gl/height.frag");
    height_shader.link();

    initialized = true;
}

void ViewportGL::makeCurrent()
{
    static_cast<QOpenGLWidget*>(view->viewport())->makeCurrent();
}

void ViewportGL::doneCurrent()
{
    static_cast<QOpenGLWidget*>(view->viewport())->doneCurrent();
}
