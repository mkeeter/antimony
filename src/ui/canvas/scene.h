#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

class Node;

class GraphScene : public QGraphicsScene
{
public:
    GraphScene(QObject* parent=0);
    void makeUIfor(Node* n);
};

#endif
