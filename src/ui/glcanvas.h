#ifndef GL_CANVAS_H
#define GL_CANVAS_H

#include <QtOpenGL/QGLWidget>
#include <QtOpenGL/QGLFunctions>
#include <QtOpenGL/QGLShaderProgram>

class GLCanvas : public QGLWidget, protected QGLFunctions
{
    Q_OBJECT
public:
    GLCanvas(const QGLFormat& format, QWidget* parent);

    void initializeGL();
    void paintEvent(QPaintEvent* event);

protected:
    QGLShaderProgram quad_shader;
};

#endif
