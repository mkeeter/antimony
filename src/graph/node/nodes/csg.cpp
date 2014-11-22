#include <Python.h>

#include "graph/node/node.h"
#include "graph/node/manager.h"

#include "graph/node/nodes/csg.h"

#include "graph/datum/datums/name_datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/shape_input_datum.h"
#include "graph/datum/datums/shape_function_datum.h"
#include "graph/datum/datums/string_datum.h"

////////////////////////////////////////////////////////////////////////////////

Node* DifferenceNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::DIFFERENCE, parent);
    new NameDatum("name", NodeManager::manager()->getName("d"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new ShapeInputDatum("a", n);
    new ShapeInputDatum("b", n);
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
    new ShapeInputDatum("a", n);
    new ShapeInputDatum("b", n);
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
    new ShapeInputDatum("a", n);
    new ShapeInputDatum("b", n);
    new ShapeFunctionDatum("shape", n, "union", {"a", "b"});
    return n;
}

Node* BlendNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::BLEND, parent);
    new NameDatum("name", NodeManager::manager()->getName("b"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new ShapeInputDatum("a", n);
    new ShapeInputDatum("b", n);
    new FloatDatum("q", "0.1", n);
    new ShapeFunctionDatum("shape", n, "blend", {"a", "b", "q"});
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
    new ShapeInputDatum("a", n);
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
    new ShapeInputDatum("a", n);
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
    new ShapeInputDatum("a", n);
    new ShapeInputDatum("b", n);
    new FloatDatum("o", "0.0", n);
    new ShapeFunctionDatum("shape", n, "clearance", {"a", "b", "o"});

    return n;
}
