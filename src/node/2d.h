#ifndef NODE_2D_H
#define NODE_2D_H

#include <QObject>
#include <QString>

class Node;

Node* CircleNode(QString name, QObject* parent=0);
Node* CircleNode(QString name, QString x, QString y, QString r, QObject* parent=0);
Node* CircleNode(float x, float y, float z, float scale, QObject* parent=0);

Node* Point2DNode(float x, float y, float z, float scale, QObject* parent=0);

Node* TextNode(float x, float y, float z, float scale, QObject* parent=0);

Node* TriangleNode(float x, float y, float z, float scale, QObject* parent=0);

#endif
