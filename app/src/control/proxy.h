#ifndef CONTROL_PROXY_H
#define CONTROL_PROXY_H

#include <QGraphicsObject>
#include <QPointer>
#include <QVector3D>
#include <QMatrix4x4>

class Control;
class Node;
class Viewport;

class ControlProxy : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit ControlProxy(Control* control, Viewport* viewport);

    /*
     *  Returns the bounding rect of the QGraphicsObject by calling
     *  Control::bounds.
     */
    QRectF boundingRect() const override;

    /*
     *  Returns a more precise shape for selection region.
     */
    QPainterPath shape() const override;

    /*
     * Paint this object by calling the control's paint function.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    /*
     *  On hover enter, set hover to true and update.
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    /*
     *  On hover leave, set hover to false and update.
     */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    /*
     *  Handle mouse clicks by preparing to drag.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    /*
     *  On mouse release, release mouse (if it was grabbed)
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    /*
     *  On mouse drag, call the virtual function drag.
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    /*
     *  On delete or backspace, delete node.
     */
    void keyPressEvent(QKeyEvent* event) override;

    /*
     *  Returns the Node that this proxy is pointing to
     *  (or NULL)
     */
    Node* getNode() const;

    /*
     *  Returns the Control that this is a proxy for.
     */
    Control* getControl() const;

    /*
     *  When selection state changes, propagate upstream.
     */
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant& value) override;

public slots:
    void redraw();
    void selectProxy(bool s);

protected:
    /*
     *  Looks up the transform matrix for this proxy's viewport.
     */
    QMatrix4x4 getMatrix() const;

    QPointer<Control> control;
    Viewport* viewport;

    bool hover;
    QPointF click_pos;

    bool changing_selection;
};

#endif // CONTROL_PROXY_H
