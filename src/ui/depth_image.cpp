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

DepthImageItem::~DepthImageItem()
{
    glDeleteTextures(1, &depth_tex);
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

        shader.addShaderFromSourceFile(QGLShader::Vertex, ":/gl/quad.vert");
        shader.addShaderFromSourceFile(QGLShader::Fragment, ":/gl/quad.frag");
        shader.link();
    }

    glGenTextures(1, &depth_tex);
    glBindTexture(GL_TEXTURE_2D, depth_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
            GL_TEXTURE_2D, 0,   /* target, level */
            GL_RGBA8,           /* format */
            depth.width(), depth.height(), 0, /* width, height border */
            GL_RGBA, GL_UNSIGNED_BYTE,   /* Data format */
            depth.bits()        /* Input data */
    );
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

    // Load vertices into shader
    const GLuint vp = shader.attributeLocation("vertex_position");
    glEnableVertexAttribArray(vp);
    glVertexAttribPointer(vp, 2, GL_FLOAT, false,
                          2 * sizeof(GLfloat), 0);

    // Load image's screen position into shader
    const GLuint offset_loc = shader.uniformLocation("offset");
    QPointF corner = canvas->mapFromScene(canvas->worldToScene(pos).toPoint());
    glUniform2f(
            offset_loc,
            2*(corner.x() - canvas->width()/2) / canvas->width(),
            -2*(corner.y() - canvas->height()/2) / canvas->height());

    // Load image's width and height into shader
    const GLuint width_loc = shader.uniformLocation("width");
    const GLuint height_loc = shader.uniformLocation("height");
    glUniform1f(
            width_loc,
            2*(size.x() * canvas->getScale()) / canvas->width());
    glUniform1f(
            height_loc,
            2*(size.y() * canvas->getScale()) / canvas->height());

    const GLuint depth_loc = shader.uniformLocation("depth_tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depth_tex);
    glUniform1i(depth_loc, 0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
