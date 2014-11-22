#ifndef NODE_TRANSFORMS_H
#define NODE_TRANSFORMS_H

#include <QObject>
#include <QString>

class Node;

Node* RotateXNode(float x, float y, float z, float scale, QObject* parent=0);
Node* RotateYNode(float x, float y, float z, float scale, QObject* parent=0);
Node* RotateZNode(float x, float y, float z, float scale, QObject* parent=0);

Node* ReflectXNode(float x, float y, float z, float scale, QObject* parent=0);
Node* ReflectYNode(float x, float y, float z, float scale, QObject* parent=0);
Node* ReflectZNode(float x, float y, float z, float scale, QObject* parent=0);

Node* RecenterNode(float x, float y, float z, float scale, QObject* parent=0);
Node* TranslateNode(float x, float y, float z, float scale, QObject* parent=0);

#endif
