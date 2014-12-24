#include <Python.h>

#include "graph/node/node.h"
#include "graph/node/root.h"

#include "graph/node/nodes/2d.h"

#include "graph/datum/datums/name_datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/shape_input_datum.h"
#include "graph/datum/datums/shape_function_datum.h"
#include "graph/datum/datums/string_datum.h"

////////////////////////////////////////////////////////////////////////////////

Node* CircleNode(QString name, NodeRoot* parent)
{
    return new Node(NodeType::CIRCLE, name, parent);
}

Node* CircleNode(QString name, QString x, QString y, QString r, NodeRoot* parent)
{
    Node* n = CircleNode(name, parent);
    new FloatDatum("x", x, n);
    new FloatDatum("y", y, n);
    new FloatDatum("r", r, n);
    new ShapeFunctionDatum("shape", n, "circle", {"x","y","r"});
    return n;
}

Node* CircleNode(float x, float y, float z, float scale, NodeRoot* parent)
{
    Q_UNUSED(z);

    return CircleNode(parent->getName("c"),
                      QString::number(x),
                      QString::number(y),
                      QString::number(scale), parent);
}

////////////////////////////////////////////////////////////////////////////////

Node* Point2DNode(float x, float y, float z, float scale, NodeRoot* parent)
{
    Q_UNUSED(z);
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::POINT2D, parent->getName("p"), parent);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* TextNode(float x, float y, float z, float scale, NodeRoot* parent)
{
    Q_UNUSED(z);

    Node* n = new Node(NodeType::TEXT, parent->getName("t"), parent);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new StringDatum("text", "hello", n);
    new FloatDatum("scale", QString::number(scale), n);
    new ShapeFunctionDatum("shape", n, "text", {"text", "x", "y", "scale"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* TriangleNode(float x, float y, float z, float scale, NodeRoot* parent)
{
    Q_UNUSED(z);

    Node* n = new Node(NodeType::TRIANGLE, parent->getName("t"), parent);

    new FloatDatum("x0", QString::number(x), n);
    new FloatDatum("y0", QString::number(y), n);
    new FloatDatum("x1", QString::number(x + scale), n);
    new FloatDatum("y1", QString::number(y), n);
    new FloatDatum("x2", QString::number(x), n);
    new FloatDatum("y2", QString::number(y + scale), n);

    new ShapeFunctionDatum("shape", n, "triangle",
            {"x0","y0","x1","y1","x2","y2"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* RectangleNode(float x, float y, float z, float scale, NodeRoot* parent)
{
    Q_UNUSED(z);

    Node* n = new Node(NodeType::RECTANGLE, parent->getName("r"), parent);

    new FloatDatum("xmin", QString::number(x), n);
    new FloatDatum("ymin", QString::number(y), n);
    new FloatDatum("xmax", QString::number(x + scale), n);
    new FloatDatum("ymax", QString::number(y + scale), n);

    new ShapeFunctionDatum("shape", n, "rectangle",
            {"xmin","xmax","ymin","ymax"});
    return n;
}
