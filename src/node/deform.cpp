#include <Python.h>

#include "node/deform.h"
#include "node/node.h"

#include "node/manager.h"

#include "datum/name_datum.h"
#include "datum/float_datum.h"
#include "datum/shape_datum.h"
#include "datum/function_datum.h"
#include "datum/string_datum.h"

////////////////////////////////////////////////////////////////////////////////

Node* AttractNode(float x, float y, float z, float scale, QObject* parent)
{
    Node* n = new Node(NodeType::ATTRACT, parent);
    new NameDatum("name", NodeManager::manager()->getName("a"), n);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new FloatDatum("z", QString::number(z), n);
    new FloatDatum("r", QString::number(scale), n);
    new ShapeDatum("input", n);
    new ShapeFunctionDatum("shape", n, "attract",
            {"input", "x", "y", "z", "r"});
    return n;
}

Node* RepelNode(float x, float y, float z, float scale, QObject* parent)
{
    Node* n = new Node(NodeType::REPEL, parent);
    new NameDatum("name", NodeManager::manager()->getName("a"), n);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new FloatDatum("z", QString::number(z), n);
    new FloatDatum("r", QString::number(scale), n);
    new ShapeDatum("input", n);
    new ShapeFunctionDatum("shape", n, "repel",
            {"input", "x", "y", "z", "r"});
    return n;
}

Node* ScaleXNode(float x, float y, float z, float scale, QObject* parent)
{
    Node* n = new Node(NodeType::SCALEX, parent);
    new NameDatum("name", NodeManager::manager()->getName("a"), n);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new FloatDatum("s", QString::number(scale), n);
    new ShapeDatum("input", n);
    new ShapeFunctionDatum("shape", n, "scale_x",
            {"input", "x", "s"});
    return n;
}

Node* ScaleYNode(float x, float y, float z, float scale, QObject* parent)
{
    Node* n = new Node(NodeType::SCALEY, parent);
    new NameDatum("name", NodeManager::manager()->getName("a"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new FloatDatum("s", QString::number(scale), n);
    new ShapeDatum("input", n);
    new ShapeFunctionDatum("shape", n, "scale_y",
            {"input", "y", "s"});
    return n;
}

Node* ScaleZNode(float x, float y, float z, float scale, QObject* parent)
{
    Node* n = new Node(NodeType::SCALEZ, parent);
    new NameDatum("name", NodeManager::manager()->getName("a"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("z", QString::number(z), n);
    new FloatDatum("s", QString::number(scale), n);
    new ShapeDatum("input", n);
    new ShapeFunctionDatum("shape", n, "scale_z",
            {"input", "z", "s"});
    return n;
}

