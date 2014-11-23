#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMatrix4x4>
#include <QPointer>

class NodeInspector;
class Link;

class Canvas : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Canvas(QWidget* parent=0);
    explicit Canvas(QGraphicsScene* scene, QWidget* parent=0);

    void setScene(QGraphicsScene* s);

#if 0
    /** Finds an input port at the given position (or NULL)
     */
    InputPort* getInputPortAt(QPointF pos) const;

    /** Returns the nearest input port.
     *  If link is given, only return input ports that accept it.
     */
    InputPort* getInputPortNear(QPointF pos, Link* link=NULL) const;

    /** Finds an inspector at the given position (or NULL)
     */
    NodeInspector* getInspectorAt(QPointF pos) const;
#endif

    QGraphicsScene* scene;

protected:
    /** On mouse press, save click position (for panning).
     */
    void mousePressEvent(QMouseEvent* event) override;

    /* On mouse drag, pan the scene (if left-click).
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /** On delete key press, delete nodes and connections.
     */
    void keyPressEvent(QKeyEvent *event) override;

    /** Draws shaded panels in the background.
     */
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    /** Pans the scene rectangle.
     */
    void pan(QVector3D d);

    QPointF click_pos;
};

#endif // CANVAS_H
