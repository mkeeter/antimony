#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QPointer>

class Node;
class Link;
class NodeInspector;
class InputPort;
class Connection;

class GraphScene : public QGraphicsScene
{
public:
    GraphScene(QObject* parent=0);
    NodeInspector* makeUIfor(Node* n);
    Connection* makeUIfor(Link* link);

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
