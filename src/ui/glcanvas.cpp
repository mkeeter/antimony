#include "ui/glcanvas.h"

GLCanvas::GLCanvas(const QGLFormat& format, QWidget* parent)
    : QGLWidget(format, parent)
{
    // Nothing to do here
}

void GLCanvas::initializeGL()
{
    initializeGLFunctions();
}

void GLCanvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    glClearColor(1.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}
