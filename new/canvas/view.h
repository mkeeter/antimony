#pragma once

#include <QGraphicsView>

class Graph;
class CanvasScene;

class CanvasView : public QGraphicsView
{
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

    QPointF click_pos;
    QPointF drag_pos;
    bool selecting;
};
