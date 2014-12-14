#include <Python.h>

#include "graph/node/node.h"
#include "graph/node/manager.h"

#include "graph/node/nodes/2d.h"

#include "graph/datum/datums/name_datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/shape_input_datum.h"
#include "graph/datum/datums/shape_function_datum.h"
#include "graph/datum/datums/string_datum.h"

////////////////////////////////////////////////////////////////////////////////

Node* CircleNode(QString name, QObject* parent)
{
    return new Node(NodeType::CIRCLE, name, parent);
}

Node* CircleNode(QString name, QString x, QString y, QString r, QObject* parent)
{
    Node* n = CircleNode(name, parent);
    new FloatDatum("x", x, n);
    new FloatDatum("y", y, n);
    new FloatDatum("r", r, n);
    new ShapeFunctionDatum("shape", n, "circle", {"x","y","r"});
    return n;
}

Node* CircleNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(z);

    return CircleNode(NodeManager::manager()->getName("c"),
                      QString::number(x),
                      QString::number(y),
                      QString::number(scale), parent);
}

////////////////////////////////////////////////////////////////////////////////

Node* Point2DNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(z);
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::POINT2D, parent);
    new NameDatum("_name", NodeManager::manager()->getName("p"), n);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* TextNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(z);

    Node* n = new Node(NodeType::TEXT, parent);
    new NameDatum("_name", NodeManager::manager()->getName("t"), n);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new StringDatum("text", "hello", n);
    new FloatDatum("scale", QString::number(scale), n);
    new ShapeFunctionDatum("shape", n, "text", {"text", "x", "y", "scale"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* TriangleNode(float x, float y, float z, float scale, QObject* parent)
{
    Node* n = new Node(NodeType::TRIANGLE, parent);
    new NameDatum("_name", NodeManager::manager()->getName("t"), n);
    Point2DNode(x, y, z, 0, n)->setObjectName("a");
    Point2DNode(x + scale, y, z, 0, n)->setObjectName("b");
    Point2DNode(x, y + scale, z, 0, n)->setObjectName("c");
    new ShapeFunctionDatum("shape", n, "triangle",
            {"a.x","a.y","b.x","b.y","c.x","c.y"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* RectangleNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(z);

    Node* n = new Node(NodeType::RECTANGLE, parent);
    new NameDatum("_name", NodeManager::manager()->getName("r"), n);
    Point2DNode(x, y, 0, 0, n)->setObjectName("a");
    Point2DNode(x + scale, y + scale, 0, 0, n)->setObjectName("b");
    new ShapeFunctionDatum("shape", n, "rectangle",
            {"a.x","b.x","a.y","b.y"});
    return n;
}
