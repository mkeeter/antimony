#ifndef NODE_3D_H
#define NODE_3D_H

#include <QString>

class Node;
class NodeRoot;

Node* CubeNode(QString name, NodeRoot* parent);
Node* CubeNode(float x, float y, float z, float scale, NodeRoot* parent);

Node* CylinderNode(float x, float y, float z, float scale, NodeRoot* parent);

Node* ExtrudeNode(float x, float y, float z, float scale, NodeRoot* parent);

Node* Point3DNode(QString name, NodeRoot* parent);
Node* Point3DNode(QString name, QString x, QString y, QString z, NodeRoot* parent);
Node* Point3DNode(float x, float y, float z, float scale, NodeRoot* parent);

Node* SphereNode(float x, float y, float z, float scale, NodeRoot* parent);

Node* ConeNode(float x, float y, float z, float scale, NodeRoot* parent);

#endif
