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
    NodeInspector* makeUIfor(Node* n);

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

protected:
    QPointer<NodeInspector> raised_inspector;
};

#endif
