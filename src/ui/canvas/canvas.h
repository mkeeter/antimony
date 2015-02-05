#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPointer>

#include "graph/node/constructor.h"

class Node;
class NodeInspector;
class Link;
class GraphScene;
namespace Ui { class MainWindow; }

class Canvas : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Canvas(QWidget* parent=0);
    explicit Canvas(GraphScene* scene, QWidget* parent=0);

    /*
     *  Connect to appropriate UI actions and modify menus.
     */
    void customizeUI(Ui::MainWindow* ui);

    void makeNodeAtCursor(NodeConstructorFunction f);

    NodeInspector* getNodeInspector(Node* n) const;

    GraphScene* scene;

public slots:
    void onCopy();
    void onCut();
    void onPaste();

protected:
    /** On mouse press, save click position (for panning).
     */
    void mousePressEvent(QMouseEvent* event) override;

    /* On mouse drag, pan the scene (if left-click).
     */
    void mouseMoveEvent(QMouseEvent* event) override;

    /*
     *  On mouse release, do the multi-select thing.
     */
    void mouseReleaseEvent(QMouseEvent* event) override;

    /*  When the scroll wheel is rolled, zoom about the cursor.
     */
    void wheelEvent(QWheelEvent* event) override;

    /** On delete key press, delete nodes and connections.
     */
    void keyPressEvent(QKeyEvent *event) override;

    /** Draws pixel grid in the background
     */
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    /*
     *  Draws selection area in the foreground.
     */
    void drawForeground(QPainter* painter, const QRectF& rect) override;

    /** Pans the scene rectangle.
     */
    void pan(QVector3D d);

    /*
     *  Deletes all selected objects in an undo-able way
     */
    void deleteSelected();

    QPointF click_pos;
    QPointF drag_pos;
    bool selecting;
};

#endif // CANVAS_H
