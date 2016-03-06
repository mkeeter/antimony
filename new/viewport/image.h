#pragma once

#include <QObject>
#include <QVector>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QImage>
#include <QPointer>
#include <QColor>

class ViewportView;
class ViewportGL;
class RenderInstance;

class DepthImage : public QObject, protected QOpenGLFunctions
{
public:
    DepthImage(RenderInstance* parent, ViewportView* view);
    ~DepthImage();

    /*
     *  Update the image settings, generating new textures
     */
    void update(QVector3D pos, QVector3D size,
                QImage depth, QImage shaded, QColor color,
                bool flat);

    /*
     *  Releases OpenGL texture objects and clears the viewport pointer
     *  (called on deletion and by the Viewport destructor)
     */
    void clearTextures();

    /*
     *  Paints the image with the given world-to-scene transform matrix
     */
    void paint(QMatrix4x4 m);

    /*
     *  Lookup functions for position and size
     */
    QVector3D getPos() const    { return pos; }
    QVector3D getSize() const   { return size; }

    /*
     *  Check if the image is valid
     */
    bool isValid() const { return valid; }
protected:
    /*
     *  Initializes OpenGL textures
     */
    void buildTexture(QImage img, GLuint* tex);

    /*
     *  Paints the given images as shaded textures
     */
    void paintShaded(QMatrix4x4 m);

    /*
     *  Paints the given images as heightmap textures
     */
    void paintHeightmap(QMatrix4x4 m);

    /*
     *  Loads variables that are shared between height-map and shaded shaders.
     *  m is the world-to-scene transform matrix
     */
    void loadSharedShaderVariables(QMatrix4x4 m, QOpenGLShaderProgram* shader);

    /*  The image is invalid until data is loaded for the first time  */
    bool valid=false;

    /*  Pointer to the viewport view that this image draws into  */
    QPointer<ViewportView> view;
    ViewportGL* gl;

    /*  Position of center (in original scene units) */
    QVector3D pos;

    /*  Scale (in rotated scene coordinates) */
    QVector3D size;

    /*  OpenGL textures  */
    GLuint depth_tex;
    GLuint shaded_tex;

    /*  Other relevant settings for drawing images  */
    QColor color;
    bool flat;
};
