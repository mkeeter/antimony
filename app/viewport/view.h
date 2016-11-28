#pragma once

#include <QVector3D>
#include <QtGui/QOpenGLFunctions>
#include <QGraphicsView>

#include "viewport/gl.h"

class DepthImage;
class Control;
class Node;
class BaseDatumProxy;
class ViewportScene;

class ViewportView : public QGraphicsView, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    ViewportView(QWidget* parent, ViewportScene* scene);

    enum MatrixParams { ROT=1, SCALE=2, MOVE=4 };

    /*
     *  Returns a generic matrix transform from the view
     */
    QMatrix4x4 getMatrix(int params=ROT|SCALE|MOVE) const;

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
     *  Override key events for menu opening
     */
    void keyPressEvent(QKeyEvent* event) override;

    /*
     *  Convert from a mouse position in scene coordinates to a world position
     */
    QVector3D sceneToWorld(QPointF pos) const;

    /*
     *  Installs a depth image (to be drawn if relevant)
     */
    void installImage(DepthImage* d);

    /*
     *  Locks yaw and pitch to the given values.
     */
    void lockAngle(float y, float p);

    /*
     *  Checks to see if the UI is hidden
     */
    bool isUIhidden() const { return ui_hidden; }

    /*  Publically accessible handle to get shaders and VBO  */
    ViewportGL gl;

signals:
    /*
     *  Emitted when the window matrix or clipping box changes
     */
    void changed(QMatrix4x4 m, QRect clip);

    /*
     *  Signal used to request that images adjust zmin and zmax based on
     *  their z bounds.
     */
    void getDepth(QMatrix4x4 m, float* zmin, float* zmax);

    /*
     *  Signal used to request that images draw themselves
     */
    void paintImage(QMatrix4x4 m, float zmin, float zmax);

    /*
     *  Signals emitted to sync up viewports in a quad view
     */
    void centerChanged(QVector3D c);
    void scaleChanged(float);

public slots:
    /*
     *  Creates controller instances for the given controller and this viewport
     */
    void installControl(Control* c);

    /*
     *  Asks the datum proxy to create a render instances for this viewport
     */
    void installDatum(BaseDatumProxy* p);

    /*
     *  Used to sync up multiple viewports in a quad window
     */
    void setCenter(QVector3D c);
    void setScale(float s);

    /*
     *  Starts an animation zooming to the given node
     */
    void zoomTo(Node* n);

    /*
     *  Shows UI elements
     */
    void hideUI(bool b);

protected:
    /*
     *  Emits a changed signal and calls QGraphicsView::update
     */
    void update();

    /*
     *  Draws X, Y, Z axes transformed with the viewport's matrix
     */
    void drawAxes(QPainter* painter) const;

    /*
     *  If we're looking down an axis, draws the coordinates in the
     *  upper-left corner of the window
     */
    void drawCoords(QPainter* painter) const;

    /*
     *  Opens a menu that allows us to add new shapes
     */
    void openAddMenu(bool view_commands=false);

    /*
     *  Opens a menu that allows us to raise an item
     */
    void openRaiseMenu(QList<QGraphicsItem*> items);

    /*
     *  Constructs a pair of QPropertyAnimations to spin the view
     */
    void spinTo(float new_yaw, float new_pitch);

    /*  Center of 3D scene  */
    QVector3D center;

    /*  Scale of 3D view (higher numbers are closer)  */
    float scale;

    /*  Angles for rotation  */
    float pitch;
    float yaw;

    /*  Properties to animate yaw and pitch with QPropertyAnimation  */
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }
    void setYaw(float y);
    void setPitch(float p);
    Q_PROPERTY(float _yaw READ getYaw WRITE setYaw)
    Q_PROPERTY(float _pitch READ getPitch WRITE setPitch)

    QVector3D getCenter() const { return center; }
    Q_PROPERTY(QVector3D _center READ getCenter WRITE setCenter)

    /*  Mouse position during a drag  */
    QPoint current_pos;

    /*  Mouse coordinates at which the click started  */
    QPointF click_pos;

    /*  Mouse click coordinates in the world's coordinate frame  */
    QVector3D click_pos_world;

    /*  Used to detect if we dragged with the mouse down or just clicked  */
    bool dragged=false;

    /*  Used to lock the angle, preventing 3D rotation  */
    bool angle_locked=false;

    /*  Pointer back to parent pseudo-scene  */
    ViewportScene* view_scene;

    /*  Pointer to raised ControlInstance  */
    QGraphicsItem* raised=nullptr;

    /*  Records whether initializeOpenGLFunctions has been called */
    bool gl_initialized=false;

    /*  Records whether the UI is hidden */
    bool ui_hidden=false;
};
