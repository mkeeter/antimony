#include <Python.h>

#include "node/2d/circle_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"

#include "control/2d/circle_control.h"

CircleNode::CircleNode(QString name, QString x, QString y, QString r,
                       QObject* parent)
    : Node(name, parent)
{
    new FloatDatum("x", x, this);
    new FloatDatum("y", y, this);
    new FloatDatum("r", r, this);
    new ShapeFunctionDatum("shape", this, "circle", {"x","y","r"});
}

CircleNode::CircleNode(float x, float y, float z, float scale, QObject* parent)
    : CircleNode(NodeManager::manager()->getName("c"),
                 QString::number(x),
                 QString::number(y),
                 QString::number(scale), parent)
{
    Q_UNUSED(z);
}

Control* CircleNode::makeControl(Canvas *canvas)
{
    return new CircleControl(canvas, this);
}
