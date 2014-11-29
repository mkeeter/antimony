#ifndef DEPTH_IMAGE_H
#define DEPTH_IMAGE_H

#include <QGraphicsObject>
#include <QImage>
#include <QVector3D>
#include <QtOpenGL/QGLFunctions>
#include <QtOpenGL/QGLShaderProgram>
#include <QtOpenGL/QGLBuffer>

class Viewport;

class DepthImageItem : public QGraphicsObject, protected QGLFunctions
{
    Q_OBJECT
public:
    DepthImageItem(QVector3D pos, QVector3D size,
                   QImage depth, QImage shaded, Viewport* viewport);
    ~DepthImageItem();
    QRectF boundingRect() const;

    /** Position of lower-left corner (in original scene units) */
    QVector3D pos;

    /** Scale (in rotated scene coordinates) */
    QVector3D size;

public slots:
    void reposition();

protected:
    void initializeGL();
    void paintShaded();
    void paintHeightmap();

    /** Loads variables that are shared between height-map and shaded shaders.
     */
    void loadSharedShaderVariables(QGLShaderProgram* shader);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QImage depth;
    QImage shaded;
    Viewport* viewport;

    GLuint depth_tex;
    GLuint shaded_tex;

    static QGLShaderProgram height_shader;
    static QGLShaderProgram shaded_shader;
    static QGLBuffer vertices;
};

#endif // DEPTH_IMAGE_H
