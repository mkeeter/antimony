#ifndef CANVAS_SCENE_H
#define CANVAS_SCENE_H

#include <QGraphicsScene>
#include <QPointer>

class Node;
class Link;

class Canvas;
class NodeInspector;
class InputPort;
class Connection;

class GraphScene : public QGraphicsScene
{
    Q_OBJECT
public:
    GraphScene(QObject* parent=0);

    /*
     *  Returns a new Canvas with scene set.
     */
    Canvas* newCanvas();

    /*
     *  Creates a new NodeInspector for the given node
     *  and adds it to the QGraphicsScene.
     */
    void makeUIfor(Node* n);

    /*
     *  Creates a new Connection for the given Link
     *  and adds it to the QGraphicsScene.
     */
    Connection* makeUIfor(Link* link);

    /*
     *  Helper function to get an item of a particular class
     *  at the given location.
     */
    template <class T>
    T* getItemAt(QPointF pos);

    NodeInspector* getInspector(Node* n);
    NodeInspector* getInspectorAt(QPointF pos);
    InputPort* getInputPortAt(QPointF pos);
    InputPort* getInputPortNear(QPointF pos, Link* link=NULL);

    void raiseInspector(NodeInspector* i);
    void raiseInspectorAt(QPointF pos);

    /*
     *  Return a list of all inspector positions, indexed by node
     *  (used in serializing the graph)
     */
    QMap<Node*, QPointF> inspectorPositions() const;

    /*
     *  Set all inspector positions from the given map.
     *  (used in deserializing the graph)
     */
    void setInspectorPositions(QMap<Node*, QPointF> p);

    /*
     *  Creates a UndoDragCommand and pushes it to the application's stack.
     */
    void endDrag(QPointF delta);

public slots:
    /*
     *  When the glow value for a node changes,
     *  propagate to the relevant inspector
     */
    void onGlowChange(Node* n, bool g);

signals:
    /*
     *  Used to cross-link glow between canvas and viewport.
     */
    void glowChanged(Node* n, bool g);

    /*
     *  Tells viewports to jump to the selected node.
     */
    void jumpTo(Node* node);

protected:
    QHash<Node*, QPointer<NodeInspector>> get_inspector_cache;
};

#endif
