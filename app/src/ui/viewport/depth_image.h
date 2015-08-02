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

class DepthImageItem : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    DepthImageItem(QVector3D pos, QVector3D size,
                   QImage depth, QImage shaded, QColor color,
                   bool flat, Viewport* viewport);
    ~DepthImageItem();

    /*
     *  Releases OpenGL texture objects and clears the viewport pointer
     *  (called on deletion and by the Viewport destructor)
     */
    void clearTextures();

    void paint();

    /** Position of center (in original scene units) */
    const QVector3D pos;

    /** Scale (in rotated scene coordinates) */
    const QVector3D size;

protected:
    void initializeGL();
    void paintShaded();
    void paintHeightmap();

    /** Loads variables that are shared between height-map and shaded shaders.
     */
    void loadSharedShaderVariables(QOpenGLShaderProgram* shader);

    QImage depth;
    QImage shaded;
    QColor color;
    bool flat;

    QPointer<Viewport> viewport;

    GLuint depth_tex;
    GLuint shaded_tex;
};

#endif // DEPTH_IMAGE_H
