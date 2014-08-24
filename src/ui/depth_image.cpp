#include <Python.h>

#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <QGLWidget>

#include "ui/depth_image.h"
#include "ui/canvas.h"

QGLShaderProgram DepthImageItem::shader;
QGLBuffer DepthImageItem::vertices;

DepthImageItem::DepthImageItem(QVector3D pos, QVector3D size, QImage depth,
                               Canvas* canvas)
    : QGraphicsObject(), pos(pos), size(size), depth(depth), canvas(canvas)
{
    connect(canvas, &Canvas::viewChanged, this, &DepthImageItem::reposition);
    reposition();
    setZValue(-20);
    initializeGL();
}

void DepthImageItem::initializeGL()
{
    initializeGLFunctions();

    if (!shader.shaders().length())
    {
        float vbuf[] = {
             0,  0,
             0,  1,
             1,  0,
             1,  1};
        vertices.create();
        vertices.bind();
        vertices.allocate(vbuf, sizeof(vbuf));
        vertices.release();

        qDebug() << "Compiling shaders!";
        shader.addShaderFromSourceFile(QGLShader::Vertex, ":/gl/quad.vert");
        shader.addShaderFromSourceFile(QGLShader::Fragment, ":/gl/quad.frag");
        shader.link();
        qDebug() << shader.log();
    }
}

void DepthImageItem::reposition()
{
    setPos(canvas->worldToScene(pos));
    prepareGeometryChange();
}

QRectF DepthImageItem::boundingRect() const
{
    return QRectF(0, 0,
                  size.x() * canvas->getScale(),
                  size.y() * canvas->getScale());
}

void DepthImageItem::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    shader.bind();
    vertices.bind();

    const GLuint vp = shader.attributeLocation("vertex_position");
    const GLuint offset_loc = shader.uniformLocation("offset");
    const GLuint width_loc = shader.uniformLocation("width");
    const GLuint height_loc = shader.uniformLocation("height");

    glEnableVertexAttribArray(vp);
    glVertexAttribPointer(vp, 2, GL_FLOAT, false,
                          2 * sizeof(GLfloat), 0);

    glUniform2f(offset_loc, 0, 0);
    glUniform1f(width_loc, 0.1);
    glUniform1f(height_loc, 0.1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
    vertices.release();
    shader.release();
    /*

    if (depth.height() == 0)
    {
        return;
    }

    // We need to transform the depth image into global z space
    QImage depth_ = depth;

    const float czmax = canvas->getZmax();
    const float czmin = canvas->getZmin();

    const float zmax = (canvas->getTransformMatrix() * pos).z();
    const float zmin = zmax - size.z();

    const int s = (zmax - zmin) / (czmax - czmin) * 0xff;
    const int o = (zmin - czmin) / (czmax - czmin) * 0xff;
    {
        QPainter p(&depth_);

        // Apply pixel scale
        QImage scale(depth.width(), depth.height(), depth.format());
        scale.fill(s | (s << 8) | (s << 16));
        p.setCompositionMode(QPainter::CompositionMode_Multiply);
        p.drawImage(0, 0, scale);

        // Apply pixel offset
        QImage offset(depth.width(), depth.height(), depth.format());
        offset.fill(o | (o << 8) | (o << 16));
        p.setCompositionMode(QPainter::CompositionMode_Plus);
        p.drawImage(0, 0, offset);

        // Multiply by a basic mask so that we have zeros outside of original
        p.setCompositionMode(QPainter::CompositionMode_Multiply);
        p.drawImage(0, 0,
                    depth.createMaskFromColor(0xff000000, Qt::MaskOutColor));
    }

    painter->setCompositionMode(QPainter::CompositionMode_Lighten);
    painter->drawImage(
            QRectF(0, 0,
                   size.x() * canvas->getScale(),
                   size.y() * canvas->getScale()),
            depth_);
    */
}
