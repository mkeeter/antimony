#include <Python.h>

#include "graph/node/node.h"
#include "graph/node/root.h"

#include "graph/node/nodes/3d.h"

#include "graph/datum/datums/name_datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/shape_input_datum.h"
#include "graph/datum/datums/shape_function_datum.h"
#include "graph/datum/datums/string_datum.h"

////////////////////////////////////////////////////////////////////////////////

Node* CubeNode(QString name, NodeRoot* parent)
{
    Node* n = new Node(NodeType::CUBE, name, parent);
    return n;
}

Node* CubeNode(float x, float y, float z, float scale, NodeRoot* parent)
{
    Node* n = new Node(NodeType::CUBE, parent->getName("c"), parent);

    new FloatDatum("xmin", QString::number(x), n);
    new FloatDatum("ymin", QString::number(y), n);
    new FloatDatum("zmin", QString::number(z), n);
    new FloatDatum("xmax", QString::number(x + scale), n);
    new FloatDatum("ymax", QString::number(y + scale), n);
    new FloatDatum("zmax", QString::number(z + scale), n);

    new ShapeFunctionDatum("shape", n, "cube",
        {"xmin", "xmax", "ymin", "ymax", "zmin", "zmax"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* CylinderNode(float x, float y, float z, float scale, NodeRoot* parent)
{
    Node* n = new Node(NodeType::CYLINDER, parent->getName("c"), parent);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new FloatDatum("z0", QString::number(z), n);
    new FloatDatum("z1", QString::number(z + scale), n);
    new FloatDatum("r", QString::number(scale / 2), n);
    new ShapeFunctionDatum("shape", n, "cylinder", {"x","y","z0","z1","r"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* ExtrudeNode(float x, float y, float z, float scale, NodeRoot* parent)
{
    Node* n = new Node(NodeType::EXTRUDE, parent->getName("e"), parent);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("z0", QString::number(z), n);
    new FloatDatum("z1", QString::number(z + scale), n);
    new FloatDatum("_scale", QString::number(scale/8), n);
    new ShapeInputDatum("input", n);
    new ShapeFunctionDatum("shape", n, "extrude_z", {"input","z0","z1"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* Point3DNode(QString name, NodeRoot* parent)
{
    Node* n = new Node(NodeType::POINT3D, name, parent);
    return n;
}

Node* Point3DNode(QString name, QString x, QString y, QString z, NodeRoot* parent)
{
    Node* n = new Node(NodeType::POINT3D, name, parent);
    new FloatDatum("x", x, n);
    new FloatDatum("y", y, n);
    new FloatDatum("z", z, n);
    return n;
}

Node* Point3DNode(float x, float y, float z, float scale, NodeRoot* parent)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::POINT3D, parent->getName("p"), parent);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new FloatDatum("z", QString::number(z), n);
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* SphereNode(float x, float y, float z, float scale, NodeRoot* parent)
{
    Node* n = new Node(NodeType::SPHERE, parent->getName("s"), parent);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new FloatDatum("z", QString::number(z), n);
    new FloatDatum("r", QString::number(scale), n);
    new ShapeFunctionDatum("shape", n, "sphere", {"x", "y", "z", "r"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* ConeNode(float x, float y, float z, float scale, NodeRoot* parent)
{
    Node* n = new Node(NodeType::CONE, parent->getName("c"), parent);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new FloatDatum("z0", QString::number(z), n);
    new FloatDatum("z1", QString::number(z + scale), n);
    new FloatDatum("r", QString::number(scale), n);
    new ShapeFunctionDatum("shape", n, "cone", {"x", "y", "z0", "z1", "r"});
    return n;
}
