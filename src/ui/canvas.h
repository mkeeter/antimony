#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMatrix4x4>

class Node;
class Control;
class InputPort;

class Canvas : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Canvas(QWidget* parent=0);

    /** Returns our scale + rotation transform matrix.
     *  (translation is handled by the QGraphicsView)
     */
    QMatrix4x4 getMatrix() const;

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

    QGraphicsScene* scene;

signals:
    void viewChanged();

protected:
    /** On mouse press, save mouse down position in _click_pos.
     *
     *  Left-clicks are saved in scene coordinates; right-clicks
     *  are saved in pixel coordinates.
     */
    virtual void mousePressEvent(QMouseEvent *event) override;

    /** Pan or spin the view.
     */
    virtual void mouseMoveEvent(QMouseEvent *event) override;

    /** On mouse wheel action, zoom about the mouse cursor.
     */
    virtual void wheelEvent(QWheelEvent *event) override;

    /** Pans the scene rectangle.
     */
    void pan(QPointF d);

    float scale;

    /* Pitch and yaw are in radians */
    float pitch;
    float yaw;

    QPointF _click_pos;

};

#endif // CANVAS_H
