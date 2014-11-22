#ifndef NODE_3D_H
#define NODE_3D_H

#include <QObject>
#include <QString>

class Node;

Node* CubeNode(QString name, QObject* parent=0);
Node* CubeNode(float x, float y, float z, float scale, QObject* parent=0);

Node* CylinderNode(float x, float y, float z, float scale, QObject* parent=0);

Node* ExtrudeNode(float x, float y, float z, float scale, QObject* parent=0);

Node* Point3DNode(QString name, QObject* parent=0);
Node* Point3DNode(QString name, QString x, QString y, QString z, QObject* parent=0);
Node* Point3DNode(float x, float y, float z, float scale, QObject* parent=0);

Node* SphereNode(float x, float y, float z, float scale, QObject* parent=0);

Node* ConeNode(float x, float y, float z, float scale, QObject* parent=0);

#endif
