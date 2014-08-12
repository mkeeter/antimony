#ifndef ITERATE_H
#define ITERATE_H

#include <QObject>
#include <QString>

class Node;

Node* Iterate2DNode(float x, float y, float z, float scale,
                    QObject* parent=0);

#endif
