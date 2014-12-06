#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMatrix4x4>
#include <QPointer>

class Node;
class Control;
class InputPort;
class NodeInspector;
class ViewSelector;
class Link;

class Viewport : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Viewport(QGraphicsScene* scene, QWidget* parent=0);

    /** Returns our scale + rotation +translate transform matrix.
     */
    QMatrix4x4 getMatrix() const;

    /** Returns a transform matrix (without pixel scale applied)
     */
    QMatrix4x4 getTransformMatrix() const;

    /** Transforms points from world to scene coordinates.
     */
    QPointF worldToScene(QVector3D v) const;
    QVector<QPointF> worldToScene(QVector<QVector3D> v) const;

    /** Transforms points from scene to world coordinates.
     */
    QVector3D sceneToWorld(QPointF p) const;

    /** Look up scale.
     */
    float getScale() const { return scale; }

    /** Gets the minimum z position of a DepthImageItem.
     */
    float getZmin() const;

    /** Gets the maximum z position of a DepthImageItem.
     */
    float getZmax() const;

    /** Get yaw (in radians)
     */
    float getYaw() const { return yaw; }

    /** Get pitch (in radians)
     */
    float getPitch() const { return pitch; }

    /** Returns the viewpoint's center.
     */
    QVector3D getCenter() const { return center; }

    QGraphicsScene* scene;
signals:
    void viewChanged();
    void showPorts(bool);

public slots:
    void spinTo(float new_yaw, float new_pitch);

protected:
    /** On mouse press, save mouse down position in _click_pos.
     *
     *  Left-clicks are saved in scene coordinates; right-clicks
     *  are saved in pixel coordinates.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /** Pan or spin the view.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /** On mouse wheel action, zoom about the mouse cursor.
     */
    void wheelEvent(QWheelEvent *event) override;

    /** On delete key press, delete nodes.
     */
    void keyPressEvent(QKeyEvent *event) override;

    /** On key release, show UI if it was alt.
     */
    void keyReleaseEvent(QKeyEvent *event) override;

    /** Clear the background (color and depth buffers)
     */
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    /*
     *  Draws the x, y, z axes.
     */
    void drawForeground(QPainter* painter, const QRectF& rect) override;

    /** Pans the scene rectangle.
     */
    void pan(QVector3D d);

    /** On resize, reposition the view selector menu
     */
    void resizeEvent(QResizeEvent* e);

    /** Hides all Control UI elements.
     */
    void hideUI();

    /** Shows all Control UI elements.
     */
    void showUI();

    QVector3D center;
    float scale;

    // Properties to automatically animate yaw and pitch (in radians)
    void setYaw(float y);
    Q_PROPERTY(float _yaw READ getYaw WRITE setYaw)
    void setPitch(float p);
    Q_PROPERTY(float _pitch READ getPitch WRITE setPitch)
    float pitch;
    float yaw;

    QPointer<Control> raised;

    QPointF _click_pos;
    QVector3D _click_pos_world;
    ViewSelector* view_selector;
};

#endif // VIEWPORT_H
