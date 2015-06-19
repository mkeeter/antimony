#include <Python.h>

#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>

#include <QOpenGLWidget>

#include "app/app.h"
#include "ui/main_window.h"
#include "ui/viewport/depth_image.h"
#include "ui/viewport/viewport.h"

DepthImageItem::DepthImageItem(QVector3D pos, QVector3D size,
                               QImage depth, QImage shaded, QColor color,
                               bool flat, Viewport* viewport)
    : QObject(viewport), pos(pos), size(size), depth(depth), shaded(shaded),
      color(color), flat(flat), viewport(viewport)
{
    initializeGL();

    // Force a redraw of the viewport when this object is created.
    viewport->scene->invalidate(QRect(), QGraphicsScene::BackgroundLayer);
}

DepthImageItem::~DepthImageItem()
{
    clearTextures();
}

void DepthImageItem::clearTextures()
{
    if (viewport)
    {
        auto v =  dynamic_cast<QOpenGLWidget*>(viewport->viewport());
        if (v && v->isValid())
        {
            static_cast<QOpenGLWidget*>(viewport->viewport())->makeCurrent();
            glDeleteTextures(1, &depth_tex);
            glDeleteTextures(1, &shaded_tex);
            static_cast<QOpenGLWidget*>(viewport->viewport())->doneCurrent();
        }
        viewport->scene->invalidate(QRect(), QGraphicsScene::BackgroundLayer);
    }
    viewport.clear();
}

void DepthImageItem::initializeGL()
{
    static_cast<QOpenGLWidget*>(viewport->viewport())->makeCurrent();

    initializeOpenGLFunctions();

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

    glGenTextures(1, &shaded_tex);
    glBindTexture(GL_TEXTURE_2D, shaded_tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
            GL_TEXTURE_2D, 0,   /* target, level */
            GL_RGBA8,           /* format */
            shaded.width(), shaded.height(), 0, /* width, height border */
            GL_RGBA, GL_UNSIGNED_BYTE,   /* Data format */
            shaded.bits()        /* Input data */
    );

    static_cast<QOpenGLWidget*>(viewport->viewport())->doneCurrent();
}

void DepthImageItem::paint()
{
    viewport->getQuadVertices()->bind();

    // Find the parent MainWindow of our Viewport
    QWidget* m = viewport;
    while (m && !dynamic_cast<MainWindow*>(m))
        m = m->parentWidget();

    if (m && dynamic_cast<MainWindow*>(m)->isShaded())
        paintShaded();
    else
        paintHeightmap();

    viewport->getQuadVertices()->release();
}

void DepthImageItem::loadSharedShaderVariables(QOpenGLShaderProgram* shader)
{
    shader->bind();

    // Load vertices into shader
    const GLuint vp = shader->attributeLocation("vertex_position");
    glEnableVertexAttribArray(vp);
    glVertexAttribPointer(vp, 2, GL_FLOAT, false,
                          2 * sizeof(GLfloat), 0);

    // Load image's screen position into shader
    const GLuint offset_loc = shader->uniformLocation("offset");
    QPointF corner = viewport->mapFromScene(viewport->worldToScene(pos).toPoint());
    glUniform2f(
            offset_loc,
            2*(corner.x() - viewport->width()/2) / viewport->width(),
            -2*(corner.y() - viewport->height()/2) / viewport->height());

    // Load image's width and height into shader
    const GLuint width_loc = shader->uniformLocation("width");
    const GLuint height_loc = shader->uniformLocation("height");
    glUniform1f(
            width_loc,
            (size.x() * viewport->getScale()) / viewport->width());
    glUniform1f(
            height_loc,
            (size.y() * viewport->getScale()) / viewport->height());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depth_tex);
    glUniform1i(shader->uniformLocation("depth_tex"), 0);

    const float zmin_global = viewport->getZmin();
    const float dz_global = viewport->getZmax() - zmin_global;

    const float dz = size.z();
    const float zmin = (viewport->getTransformMatrix() * pos).z() - dz/2;

    // Set z values for depth blending.
    glUniform1f(shader->uniformLocation("dz_local"), dz);
    glUniform1f(shader->uniformLocation("zmin_local"), zmin);
    glUniform1f(shader->uniformLocation("dz_global"), dz_global);
    glUniform1f(shader->uniformLocation("zmin_global"), zmin_global);
    glUniform1i(shader->uniformLocation("is_2d"), flat);

    glUniform3f(shader->uniformLocation("color"),
                color.redF(), color.greenF(), color.blueF());
}

void DepthImageItem::paintShaded()
{
    auto shaded_shader = viewport->getShadedShader();
    glEnable(GL_DEPTH_TEST);
    loadSharedShaderVariables(shaded_shader);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, shaded_tex);
    glUniform1i(shaded_shader->uniformLocation("shaded_tex"), 1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    shaded_shader->release();
    glDisable(GL_DEPTH_TEST);
}

void DepthImageItem::paintHeightmap()
{
    auto height_shader = viewport->getHeightmapShader();

    glEnable(GL_BLEND);
    glBlendEquation(GL_MAX);
    loadSharedShaderVariables(height_shader);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    height_shader->release();

    glDisable(GL_BLEND);
}
