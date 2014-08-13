#include <Python.h>

#include "node/csg.h"
#include "node/node.h"

#include "node/manager.h"

#include "datum/name_datum.h"
#include "datum/float_datum.h"
#include "datum/shape_datum.h"
#include "datum/function_datum.h"
#include "datum/string_datum.h"

////////////////////////////////////////////////////////////////////////////////

Node* DifferenceNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::DIFFERENCE, parent);
    new NameDatum("name", NodeManager::manager()->getName("d"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new ShapeDatum("a", n);
    new ShapeDatum("b", n);
    new ShapeFunctionDatum("shape", n, "difference", {"a", "b"});
    return n;
}

Node* IntersectionNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::INTERSECTION, parent);
    new NameDatum("name", NodeManager::manager()->getName("d"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new ShapeDatum("a", n);
    new ShapeDatum("b", n);
    new ShapeFunctionDatum("shape", n, "intersection", {"a", "b"});
    return n;
}

Node* UnionNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::UNION, parent);
    new NameDatum("name", NodeManager::manager()->getName("d"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new ShapeDatum("a", n);
    new ShapeDatum("b", n);
    new ShapeFunctionDatum("shape", n, "union", {"a", "b"});
    return n;
}

Node* OffsetNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::OFFSET, parent);
    new NameDatum("name", NodeManager::manager()->getName("d"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new ShapeDatum("a", n);
    new FloatDatum("o", "0.0", n);
    new ShapeFunctionDatum("shape", n, "offset", {"a", "o"});

    return n;
}

Node* ShellNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::SHELL, parent);
    new NameDatum("name", NodeManager::manager()->getName("d"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new ShapeDatum("a", n);
    new FloatDatum("s", "0.0", n);
    new ShapeFunctionDatum("shape", n, "shell", {"a", "s"});

    return n;
}



Node* ClearanceNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::CLEARANCE, parent);
    new NameDatum("name", NodeManager::manager()->getName("d"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new ShapeDatum("a", n);
    new ShapeDatum("b", n);
    new FloatDatum("o", "0.0", n);
    new ShapeFunctionDatum("shape", n, "clearance", {"a", "b", "o"});

    return n;
}
