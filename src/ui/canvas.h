#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMatrix4x4>

class Node;
class Control;
class InputPort;
class NodeInspector;

class Canvas : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Canvas(QWidget* parent=0);

    /** Returns our scale + rotation transform matrix.
     *  (translation is handled by the QGraphicsView)
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

    /** Finds the control associated with the given node.
     */
    Control* getControl(Node* node) const;

    /** Finds an input port at the given position (or NULL)
     */
    InputPort* getInputPortAt(QPointF pos) const;

    /** Finds an inspector at the given position (or NULL)
     */
    NodeInspector* getInspectorAt(QPointF pos) const;

    /** Gets the minimum z position of a DepthImageItem.
     */
    float getZmin() const;

    /** Gets the maximum z position of a DepthImageItem.
     */
    float getZmax() const;

    QGraphicsScene* scene;

    /** Get yaw (in radians)
     */
    float getYaw() const { return yaw; }

    /** Get pitch (in radians)
     */
    float getPitch() const { return pitch; }

    /** Returns the viewpoint's center.
     */
    QVector3D getCenter() const { return center; }

signals:
    void viewChanged();

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

    /** On delete key press, delete nodes and connections.
     */
    void keyPressEvent(QKeyEvent *event) override;

    /** Draws shaded panels in the background.
     */
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    /** On paint event, resize depth canvas then call default painter.
     */
    void paintEvent(QPaintEvent *event);

    // Properties to automatically animate yaw and pitch.
    void setYaw(float y);
    Q_PROPERTY(float _yaw READ getYaw WRITE setYaw)

    void setPitch(float p);
    Q_PROPERTY(float _pitch READ getPitch WRITE setPitch)

    /** Pans the scene rectangle.
     */
    void pan(QVector3D d);

    QVector3D center;
    float scale;

    /* Pitch and yaw are in radians */
    float pitch;
    float yaw;

    QPointF _click_pos;
    QVector3D _click_pos_world;
};

#endif // CANVAS_H
