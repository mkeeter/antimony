#ifndef POINT2D_H
#define POINT2D_H

#include <Python.h>
#include <QObject>

#include "node/node.h"

class Canvas;

class Point2D : public Node
{
public:
    explicit Point2D(QObject* parent=NULL);
    explicit Point2D(float x, float y, float z, float scale,
                     QObject* parent=NULL);

    NodeType::NodeType getNodeType() const override
        { return NodeType::POINT2D; }
};

#endif // POINT2D_H
