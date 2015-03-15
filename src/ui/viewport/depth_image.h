#ifndef DEPTH_IMAGE_H
#define DEPTH_IMAGE_H

#include <QGraphicsObject>
#include <QImage>
#include <QColor>
#include <QPointer>
#include <QVector3D>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>

class Viewport;

class DepthImageItem : public QGraphicsObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    DepthImageItem(QVector3D pos, QVector3D size,
                   QImage depth, QImage shaded, QColor color,
                   Viewport* viewport);
    ~DepthImageItem();
    QRectF boundingRect() const;


    /** Position of lower-left corner (in original scene units) */
    const QVector3D pos;

    /** Scale (in rotated scene coordinates) */
    const QVector3D size;

public slots:
    void reposition();

protected:
    void initializeGL();
    void paintShaded();
    void paintHeightmap();

    /** Loads variables that are shared between height-map and shaded shaders.
     */
    void loadSharedShaderVariables(QOpenGLShaderProgram* shader);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QImage depth;
    QImage shaded;
    QColor color;

    QPointer<Viewport> viewport;

    GLuint depth_tex;
    GLuint shaded_tex;
};

#endif // DEPTH_IMAGE_H
