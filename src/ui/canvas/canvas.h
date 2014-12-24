#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPointer>

class Node;
class NodeInspector;
class Link;

class Canvas : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Canvas(QWidget* parent=0);
    explicit Canvas(QGraphicsScene* scene, QWidget* parent=0);

    NodeInspector* getNodeInspector(Node* n) const;

    QGraphicsScene* scene;

protected:
    /** On mouse press, save click position (for panning).
     */
    void mousePressEvent(QMouseEvent* event) override;

    /* On mouse drag, pan the scene (if left-click).
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /*  When the scroll wheel is rolled, zoom about the cursor.
     */
    void wheelEvent(QWheelEvent* event) override;

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
