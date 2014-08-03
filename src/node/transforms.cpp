#include <Python.h>

#include "node/2d.h"
#include "node/node.h"

#include "node/manager.h"

#include "datum/name_datum.h"
#include "datum/float_datum.h"
#include "datum/shape_datum.h"
#include "datum/function_datum.h"
#include "datum/string_datum.h"

////////////////////////////////////////////////////////////////////////////////

Node* RotateXNode(float x, float y, float z, float scale, QObject* parent=0)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::ROTATEX, parent);
    new NameDatum("name", NodeManager::manager()->getName("r"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new FloatDatum("z", QString::number(z), n);
    new FloatDatum("a", QString::number(45), n);
    new ShapeDatum("input", n);
    new ShapeFunctionDatum("shape", n, "rotate_x",
            {"input", "a", "y", "z"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* RotateYNode(float x, float y, float z, float scale, QObject* parent=0)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::ROTATEY, parent);
    new NameDatum("name", NodeManager::manager()->getName("r"), n);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("z", QString::number(z), n);
    new FloatDatum("a", QString::number(45), n);
    new ShapeDatum("input", n);
    new ShapeFunctionDatum("shape", n, "rotate_y",
            {"input", "a", "x", "z"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* RotateZNode(float x, float y, float z, float scale, QObject* parent=0)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::ROTATEZ, parent);
    new NameDatum("name", NodeManager::manager()->getName("r"), n);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new FloatDatum("a", QString::number(45), n);
    new ShapeDatum("input", n);
    new ShapeFunctionDatum("shape", n, "rotate_z",
            {"input", "a", "x", "y"});
    return n;
}
