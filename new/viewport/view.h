#pragma once

#include <QVector3D>
#include <QGraphicsView>

#include "viewport/gl.h"

class DepthImage;
class Control;
class BaseDatumProxy;
class ViewportScene;

class ViewportView : public QGraphicsView
{
    Q_OBJECT
public:
    ViewportView(QWidget* parent, ViewportScene* scene);

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
     *  On right-click release (if we didn't drag), open add menu
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

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
     *  Opens a menu that allows us to add new shapes
     */
    void openAddMenu();

    /*  Publically accessible handle to get shaders and VBO  */
    ViewportGL gl;

signals:
    void changed(QMatrix4x4 m);

    /*
     *  Signal used to request that images adjust zmin and zmax based on
     *  their z bounds.
     */
    void getDepth(QMatrix4x4 m, float* zmin, float* zmax);

    /*
     *  Signal used to request that images draw themselves
     */
    void paintImage(QMatrix4x4 m, float zmin, float zmax);

public slots:
    /*
     *  Creates controller instances for the given controller and this viewport
     */
    void installControl(Control* c);

    /*
     *  Asks the datum proxy to create a render instances for this viewport
     */
    void installDatum(BaseDatumProxy* p);

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

    /*  Used to detect if we dragged with the mouse down or just clicked  */
    bool dragged=false;

    /*  Pointer back to parent pseudo-scene  */
    ViewportScene* view_scene;
};
