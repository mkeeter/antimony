#include <Python.h>

#include <QApplication>
#include <QDebug>
#include <QBuffer>

#include "node/manager.h"
#include "node/node.h"
#include "node/proxy.h"
#include "node/serializer.h"
#include "node/deserializer.h"

#include "datum/datum.h"
#include "datum/name_datum.h"

NodeManager* NodeManager::_manager = NULL;

NodeManager::NodeManager(QObject *parent) :
    QObject(parent)
{
    // Nothing to do here
}

bool NodeManager::isNameUnique(QString name) const
{
    PyObject* n = PyUnicode_FromString(name.toStdString().c_str());
    bool result = (findMatchingName(n) == NULL);
    Py_DECREF(n);
    return result;
}

NameDatum* NodeManager::findMatchingName(PyObject* proposed) const
{
    for (NameDatum* d : findChildren<NameDatum*>("name"))
    {
        if (d->getValid() &&
            PyObject_RichCompareBool(d->getValue(), proposed, Py_EQ))
        {
            return d;
        }
    }
    return NULL;
}


QString NodeManager::getName(QString prefix) const
{
    QString name;
    int i = 0;
    while (true)
    {
        name = prefix + QString::number(i++);
        if (isNameUnique(name))
        {
            break;
        }
    }
    return name;
}

PyObject* NodeManager::proxyDict(Datum* caller)
{
    PyObject* d = PyDict_New();
    PyDict_SetItemString(d, "__builtins__", PyEval_GetBuiltins());
    PyDict_SetItemString(d, "math", PyImport_ImportModule("math"));
    for (Node* n : findChildren<Node*>())
    {
        NameDatum* name = n->getDatum<NameDatum>("name");

        if (name->getValid())
        {
            PyObject* proxy = n->proxy();
            ((NodeProxyObject*)proxy)->caller = caller;
            PyDict_SetItem(d, name->getValue(), proxy);
        }
    }
    return d;
}

void NodeManager::clear()
{
    for (auto n : findChildren<Node*>(QString(), Qt::FindDirectChildrenOnly))
    {
        delete n;
    }
}

NodeManager* NodeManager::manager()
{
    if (_manager == NULL)
    {
        _manager = new NodeManager(QApplication::instance());
    }
    return _manager;
}

void NodeManager::onNameChange(QString new_name)
{
    // When a node's name changes, call update on any EvalDatums that
    // contain the new name as a substring.
    for (EvalDatum* e : findChildren<EvalDatum*>())
    {
        if (e->getExpr().indexOf(new_name) != -1)
        {
            e->update();
        }
    }

}

QByteArray NodeManager::getSerializedScene() const
{
    QBuffer buffer;
    buffer.open(QBuffer::WriteOnly);

    QDataStream stream(&buffer);
    SceneSerializer ss;
    ss.run(&stream);
    buffer.seek(0);

    return buffer.data();
}

bool NodeManager::deserializeScene(QByteArray in)
{
    QBuffer buffer(&in);
    buffer.open(QBuffer::ReadOnly);

    QDataStream stream(&buffer);
    SceneDeserializer ss;
    ss.run(&stream);

    return true;
}

#ifdef ANTIMONY

#include "ui/connection.h"

#include "control/2d/circle_control.h"
#include "control/2d/triangle_control.h"
#include "control/2d/text_control.h"
#include "control/2d/point2d_control.h"
#include "control/3d/cube_control.h"
#include "control/3d/sphere_control.h"
#include "control/3d/cylinder_control.h"
#include "control/3d/extrude_control.h"
#include "control/3d/point3d_control.h"
#include "control/meta/script_control.h"
#include "control/csg/union_control.h"
#include "control/csg/intersection_control.h"
#include "control/csg/difference_control.h"
#include "control/deform/attract_control.h"
#include "control/deform/repel_control.h"
#include "control/deform/scalex_control.h"
#include "control/deform/scaley_control.h"
// CONTROL HEADERS

Control* NodeManager::makeControlFor(Canvas* canvas, Node* n)
{
   switch (n->getNodeType())
    {
        case NodeType::CIRCLE:
            return new CircleControl(canvas, n);
        case NodeType::TRIANGLE:
            return new TriangleControl(canvas, n);
        case NodeType::POINT2D:
            return new Point2DControl(canvas, n);
        case NodeType::CUBE:
            return new CubeControl(canvas, n);
        case NodeType::SPHERE:
            return new SphereControl(canvas, n);
        case NodeType::CYLINDER:
            return new CylinderControl(canvas, n);
        case NodeType::EXTRUDE:
            return new ExtrudeControl(canvas, n);
        case NodeType::POINT3D:
            return new Point3DControl(canvas, n);
        case NodeType::SCRIPT:
            return new ScriptControl(canvas, n);
        case NodeType::TEXT:
            return new TextControl(canvas, n);
        case NodeType::UNION:
            return new UnionControl(canvas, n);
        case NodeType::INTERSECTION:
            return new IntersectionControl(canvas, n);
        case NodeType::DIFFERENCE:
            return new DifferenceControl(canvas, n);
        case NodeType::ATTRACT:
            return new AttractControl(canvas, n);
        case NodeType::REPEL:
            return new RepelControl(canvas, n);
        case NodeType::SCALEX:
            return new ScaleXControl(canvas, n);
        case NodeType::SCALEY:
            return new ScaleYControl(canvas, n);
        // CONTROL CASES
    }
   Q_ASSERT(false);
   return NULL;
}

void NodeManager::makeControls(Canvas* canvas)
{
    for (auto n : findChildren<Node*>(QString(), Qt::FindDirectChildrenOnly))
    {
        makeControlFor(canvas, n);
    }
}

void NodeManager::makeConnections(Canvas* canvas)
{
    for (auto link : findChildren<Link*>())
    {
        new Connection(link, canvas);
    }
}

#endif
