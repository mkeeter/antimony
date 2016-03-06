#pragma once

#include <QVector3D>
#include <QGraphicsView>

#include "viewport/gl.h"

class DepthImage;

class ViewportView : public QGraphicsView
{
    Q_OBJECT
public:
    ViewportView(QWidget* parent);

    /*
     *  Returns a generic matrix transform from the view
     */
    QMatrix4x4 getMatrix() const;

    /*
     *  Draw depth images in the background
     */
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    /*
     *  Draws foreground info (including axes)
     */
    void drawForeground(QPainter* painter, const QRectF& rect) override;

    /*
     *  On mouse move, drag or rotate the view
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /*
     *  On mouse click, save click_pos and click_pos_world
     */
    void mousePressEvent(QMouseEvent* event) override;

    /*
     *  On mouse wheel, scroll
     */
    void wheelEvent(QWheelEvent* event) override;

    /*
     *  On resize, update scene rectangle size
     */
    void resizeEvent(QResizeEvent* e) override;

    /*
     *  Convert from a mouse position in scene coordinates to a world position
     */
    QVector3D sceneToWorld(QPointF pos) const;

    /*
     *  Installs a depth image (to be drawn if relevant)
     */
    void installImage(DepthImage* d);

    /*
     *  Returns the smallest and largest depth image Z values
     */
    float getZmin() const;
    float getZmax() const;

    /*  Publically accessible handle to get shaders and VBO  */
    ViewportGL gl;

signals:
    void changed(QMatrix4x4 m);

protected:
    /*
     *  Emits a changed signal and calls QGraphicsView::update
     */
    void update();

    /*
     *  Draws X, Y, Z axes transformed with the viewport's matrix
     */
    void drawAxes(QPainter* painter) const;

    /*  Center of 3D scene  */
    QVector3D center;

    /*  Scale of 3D view (higher numbers are closer)  */
    float scale;

    /*  Angles for rotation  */
    float pitch;
    float yaw;

    /*  Mouse position during a drag  */
    QPoint current_pos;

    /*  Mouse coordinates at which the click started  */
    QPointF click_pos;

    /*  Mouse click coordinates in the world's coordinate frame  */
    QVector3D click_pos_world;

    /*  Depth images to draw  */
    QList<DepthImage*> images;
};
