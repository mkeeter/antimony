#include <Python.h>

#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>

#include <QOpenGLWidget>

#include "app/app.h"
#include "window/base.h"
#include "viewport/image.h"
#include "viewport/view.h"
#include "viewport/render/instance.h"

#include "window/viewport.h"

DepthImage::DepthImage(RenderInstance* parent, ViewportView* view)
    : QObject(parent), view(view), gl(&view->gl)
{
    gl->makeCurrent();
    initializeOpenGLFunctions();
    gl->doneCurrent();

    view->installImage(this);
}

DepthImage::~DepthImage()
{
    clearTextures();
}

void DepthImage::update(QVector3D pos_, QVector3D size_,
                        QImage depth, QImage shaded, QColor color_,
                        bool flat_)
{
    clearTextures();
    buildTexture(depth, &depth_tex);
    buildTexture(shaded, &shaded_tex);

    pos = pos_;
    size = size_;
    color = color_;
    flat = flat_;

    // Force a redraw of the viewport
    valid = true;
    view->scene()->invalidate(QRect(), QGraphicsScene::BackgroundLayer);
}

void DepthImage::getDepth(QMatrix4x4 m, float* zmin, float* zmax)
{
    m.scale(1 / sqrt(pow(m(0,0), 2) + pow(m(0, 1), 2) + pow(m(0, 2), 2)));
    *zmin = fmin(*zmin, (m * pos).z() - size.z()/2);
    *zmax = fmax(*zmax, (m * pos).z() + size.z()/2);
}

void DepthImage::clearTextures()
{
    if (view)
    {
        gl->makeCurrent();
        glDeleteTextures(1, &depth_tex);
        glDeleteTextures(1, &shaded_tex);
        gl->doneCurrent();

        valid = false;
        view->scene()->invalidate(QRect(), QGraphicsScene::BackgroundLayer);
    }
}

void DepthImage::buildTexture(QImage img, GLuint* tex)
{
    gl->makeCurrent();

    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_2D, *tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
            GL_TEXTURE_2D, 0,   /* target, level */
            GL_RGBA8,           /* format */
            img.width(), img.height(), 0, /* width, height border */
            GL_RGBA, GL_UNSIGNED_BYTE,   /* Data format */
            img.bits()        /* Input data */
    );

    gl->doneCurrent();
}

void DepthImage::paint(QMatrix4x4 m, float zmin, float zmax)
{
    if (valid)
    {
        gl->getQuadVertices()->bind();

        // Find the parent MainWindow of our Viewport
        QWidget* p = view;
        while (p && !dynamic_cast<BaseViewportWindow*>(p))
            p = p->parentWidget();

        if (p && static_cast<BaseViewportWindow*>(p)->isShaded())
        {
            paintShaded(m, zmin, zmax);
        }
        else
        {
            paintHeightmap(m, zmin, zmax);
        }

        gl->getQuadVertices()->release();
    }
}

void DepthImage::loadSharedShaderVariables(
        QMatrix4x4 m, QOpenGLShaderProgram* shader,
        float zmin_global, float zmax_global)
{
    float scale = sqrt(pow(m(0, 0), 2) + pow(m(0, 1), 2) + pow(m(0, 2), 2));
    shader->bind();

    // Load vertices into shader
    const GLuint vp = shader->attributeLocation("vertex_position");
    glEnableVertexAttribArray(vp);
    glVertexAttribPointer(vp, 2, GL_FLOAT, false,
                          2 * sizeof(GLfloat), 0);

    // Load image's screen position into shader
    const GLuint offset_loc = shader->uniformLocation("offset");
    QPointF center = view->mapFromScene((m * pos).toPoint());

    glUniform2f(
            offset_loc,
             2*(center.x() - view->width()/2) / view->width(),
            -2*(center.y() - view->height()/2) / view->height());

    // Load image's width and height into shader
    const GLuint width_loc = shader->uniformLocation("width");
    const GLuint height_loc = shader->uniformLocation("height");
    glUniform1f(width_loc, (size.x() * scale) / view->width());
    glUniform1f(height_loc, (size.y() * scale) / view->height());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depth_tex);
    glUniform1i(shader->uniformLocation("depth_tex"), 0);

    const float dz_global = zmax_global - zmin_global;

    const float dz_local= size.z();
    m.scale(1 / sqrt(pow(m(0,0), 2) + pow(m(0, 1), 2) + pow(m(0, 2), 2)));
    const float zmin_local = (m * pos).z() - dz_local/2;

    // Set z values for depth blending.
    glUniform1f(shader->uniformLocation("dz_local"), dz_local);
    glUniform1f(shader->uniformLocation("zmin_local"), zmin_local);
    glUniform1f(shader->uniformLocation("dz_global"), dz_global);
    glUniform1f(shader->uniformLocation("zmin_global"), zmin_global);
    glUniform1i(shader->uniformLocation("is_2d"), flat);

    glUniform3f(shader->uniformLocation("color"),
                color.redF(), color.greenF(), color.blueF());
}

void DepthImage::paintShaded(
        QMatrix4x4 m, float zmin_global, float zmax_global)
{
    auto shaded_shader = gl->getShadedShader();
    glEnable(GL_DEPTH_TEST);
    loadSharedShaderVariables(m, shaded_shader, zmin_global, zmax_global);

    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, shaded_tex);
    glUniform1i(shaded_shader->uniformLocation("shaded_tex"), 1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    shaded_shader->release();
    glDisable(GL_DEPTH_TEST);
}

void DepthImage::paintHeightmap(
        QMatrix4x4 m, float zmin_global, float zmax_global)
{
    auto height_shader = gl->getHeightmapShader();

    glEnable(GL_BLEND);
    glBlendEquation(GL_MAX);
    loadSharedShaderVariables(m, height_shader, zmin_global, zmax_global);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    height_shader->release();

    glDisable(GL_BLEND);
}
