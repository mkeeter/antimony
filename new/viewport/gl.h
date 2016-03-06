#pragma once

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLWidget>

class ViewportView;

class ViewportGL
{
public:
    /*
     *  Construct the GL object, storing the view
     *  so that we can set its context as current
     */
    ViewportGL(QOpenGLWidget* w) : context(w) {};

    /*
     *  Returns the shaders for shaded and heightmap rendering, respectively
     */
    QOpenGLShaderProgram* getShadedShader();
    QOpenGLShaderProgram* getHeightmapShader();

    /*
     *  Returns the VBO for drawing textured quads
     */
    QOpenGLBuffer* getQuadVertices();

    /*
     *  Make the relevant context current
     */
    void makeCurrent();

    /*
     *  Finish the current context operations
     */
    void doneCurrent();

protected:
    /*
     *  Initialize the shader and VBO
     */
    void initialize();

    /*  The ViewportView is a subclass of QGraphicsView.          *
     *  It better have been constructed with an OpenGL viewport!  */
    QOpenGLWidget* context;

    bool initialized=false;

    QOpenGLShaderProgram height_shader;
    QOpenGLShaderProgram shaded_shader;
    QOpenGLBuffer quad_vertices;

    friend class ViewportView;
};
