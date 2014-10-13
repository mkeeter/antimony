#include <Python.h>

#include "node/3d.h"
#include "node/node.h"

#include "node/manager.h"

#include "datum/name_datum.h"
#include "datum/float_datum.h"
#include "datum/shape_datum.h"
#include "datum/function_datum.h"
#include "datum/string_datum.h"

////////////////////////////////////////////////////////////////////////////////

Node* CubeNode(QString name, QObject* parent)
{
    Node* n = new Node(NodeType::CUBE, parent);
    new NameDatum("name", name, n);
    return n;
}

Node* CubeNode(float x, float y, float z, float scale, QObject* parent)
{
    Node* n = new Node(NodeType::CUBE, parent);
    new NameDatum("name", NodeManager::manager()->getName("c"), n);
    Point3DNode(x, y, z, 0, n)->setObjectName("a");
    Point3DNode(x + scale, y + scale, z + scale, 0, n)->setObjectName("b");
    new ShapeFunctionDatum("shape", n, "cube",
        {"a.x", "b.x", "a.y", "b.y", "a.z", "b.z"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* CylinderNode(float x, float y, float z, float scale, QObject* parent)
{
    Node* n = new Node(NodeType::CYLINDER, parent);
    new NameDatum("name", NodeManager::manager()->getName("c"), n);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new FloatDatum("z0", QString::number(z), n);
    new FloatDatum("z1", QString::number(z + scale), n);
    new FloatDatum("r", QString::number(scale / 2), n);
    new ShapeFunctionDatum("shape", n, "cylinder", {"x","y","z0","z1","r"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* ExtrudeNode(float x, float y, float z, float scale, QObject* parent)
{
    Node* n = new Node(NodeType::EXTRUDE, parent);
    new NameDatum("name", NodeManager::manager()->getName("e"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("z0", QString::number(z), n);
    new FloatDatum("z1", QString::number(z + scale), n);
    new FloatDatum("_scale", QString::number(scale/8), n);
    new ShapeDatum("input", n);
    new ShapeFunctionDatum("shape", n, "extrude_z", {"input","z0","z1"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* Point3DNode(QString name, QObject* parent)
{
    Node* n = new Node(NodeType::POINT3D, parent);
    new NameDatum("name", name, n);
    return n;
}

Node* Point3DNode(QString name, QString x, QString y, QString z, QObject* parent)
{
    Node* n = new Node(NodeType::POINT3D, parent);
    new NameDatum("name", name, n);
    new FloatDatum("x", x, n);
    new FloatDatum("y", y, n);
    new FloatDatum("z", z, n);
    return n;
}

Node* Point3DNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::POINT3D, parent);
    new NameDatum("name", NodeManager::manager()->getName("p"), n);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new FloatDatum("z", QString::number(z), n);
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* SphereNode(float x, float y, float z, float scale, QObject* parent)
{
    Node* n = new Node(NodeType::SPHERE, parent);
    new NameDatum("name", NodeManager::manager()->getName("s"), n);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new FloatDatum("z", QString::number(z), n);
    new FloatDatum("r", QString::number(scale), n);
    new ShapeFunctionDatum("shape", n, "sphere", {"x", "y", "z", "r"});
    return n;
}

////////////////////////////////////////////////////////////////////////////////

Node* ConeNode(float x, float y, float z, float scale, QObject* parent)
{
    Node* n = new Node(NodeType::CONE, parent);
    new NameDatum("name", NodeManager::manager()->getName("c"), n);
    new FloatDatum("x", QString::number(x), n);
    new FloatDatum("y", QString::number(y), n);
    new FloatDatum("z0", QString::number(z), n);
    new FloatDatum("z1", QString::number(z + scale), n);
    new FloatDatum("r", QString::number(scale), n);
    new ShapeFunctionDatum("shape", n, "cone", {"x", "y", "z0", "z1", "r"});
    return n;
}
