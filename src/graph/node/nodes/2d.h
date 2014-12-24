#ifndef NODE_2D_H
#define NODE_2D_H

#include <QObject>
#include <QString>

class Node;
class NodeRoot;

Node* CircleNode(QString name, NodeRoot* parent);
Node* CircleNode(QString name, QString x, QString y, QString r, NodeRoot* parent);
Node* CircleNode(float x, float y, float z, float scale, NodeRoot* parent);

Node* Point2DNode(float x, float y, float z, float scale, NodeRoot* parent);

Node* TextNode(float x, float y, float z, float scale, NodeRoot* parent);

Node* TriangleNode(float x, float y, float z, float scale, NodeRoot* parent);

Node* RectangleNode(float x, float y, float z, float scale, NodeRoot* parent);

#endif
