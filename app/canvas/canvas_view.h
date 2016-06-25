#pragma once

#include <QGraphicsView>

class Graph;
class Node;
class Datum;

class CanvasScene;
class InspectorFrame;

class CanvasView : public QGraphicsView
{
    Q_OBJECT

public:
    CanvasView(CanvasScene* scene, QWidget* parent);

public slots:
    /*
     *  Loads either text (if a datum editor is focused) or a set of nodes
     *  to the system clipboard
     */
    void onCopy();
    void onCut();

    /*
     *  Pastes either text (if a datum editor is focused) or a set of nodes
     */
    void onPaste();

    /*
     *  Starts an animation zooming to the given node
     */
    void zoomTo(Node* n);

protected:
    /*
     *  Overload draw events for background and selection rectangle.
     */
    void drawBackground(QPainter* painter, const QRectF& rect) override;
    void drawForeground(QPainter* painter, const QRectF& rect) override;

    /*
     *  Overload mouse events for dragging, zooming, panning, etc.
     */
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    /*
     *  Override key events for deletion and menu opening
     */
    void keyPressEvent(QKeyEvent* event) override;

    /*
     *  Creates an UndoDeleteMulti that deletes all selected objects
     */
    void deleteSelected();

    /*
     *  Paste in an array of nodes, properly updating UIDs and names
     */
    void pasteNodes(QJsonArray array);

    /*
     *  Grabs the given node or datum
     *  (used when inserting things into the canvas)
     */
    template <class T>
    void grab(T* t);

    /*
     *  Opens a menu that allows us to add new shapes
     */
    void openAddMenu();

    /*
     *  Looks up the inspector for the given node
     */
    InspectorFrame* inspectorFor(Node* n);

    void grabNode(Node* n);
    void grabDatum(Datum* n);

    /*  Properties used to animate zooming to a particular node  */
    QPointF getCenter() const;
    void setCenter(QPointF p);
    Q_PROPERTY(QPointF _center READ getCenter WRITE setCenter)
    void setZoom(float z);
    float getZoom() const;
    Q_PROPERTY(float _zoom READ getZoom WRITE setZoom)

    QPointF click_pos;
    QPointF drag_pos;
    bool selecting;
};
