#include <Python.h>

#include <QStringList>

#include "node/node.h"
#include "node/manager.h"
#include "node/proxy.h"

#include "datum/datum.h"
#include "datum/name_datum.h"

Node::Node(NodeType::NodeType type, QObject* parent)
    : QObject(parent), type(type), control(NULL)
{
    if (parent == NULL)
    {
        setParent(NodeManager::manager());
    }
}

Node::Node(NodeType::NodeType type, QString name, QObject* parent)
    : Node(type, parent)
{
    new NameDatum("name", name, this);
    if (parent == NULL)
    {
        setParent(NodeManager::manager());
    }
}


PyObject* Node::proxy()
{
    PyObject* p = PyObject_CallObject(proxyType(), NULL);
    Q_ASSERT(p);
    ((NodeProxyObject*)p)->node = this;
    ((NodeProxyObject*)p)->caller = NULL;
    return p;
}

Datum* Node::getDatum(QString name) const
{
    QStringList s = name.split(".");

    if (s.length() == 1)
    {
        return findChild<Datum*>(s.back());
    }
    else
    {
        Node* n = findChild<Node*>(s.front());
        if (n)
        {
            s.pop_front();
            return n->getDatum(s.join("."));
        }
    }
    return NULL;
}

QString Node::getName() const
{
    auto e = getDatum<EvalDatum>("name");
    if (e)
    {
        return e->getExpr();
    }
    return "";
}

QString Node::getType() const
{
    switch (type)
    {
        case NodeType::CIRCLE:      return "Circle";
        case NodeType::TRIANGLE:    return "Triangle";
        case NodeType::POINT2D:     return "Point (2D)";
        case NodeType::RECTANGLE:   return "Rectangle";
        case NodeType::TEXT:        return "Text";
        case NodeType::CUBE:        return "Cube";
        case NodeType::CYLINDER:    return "Cylinder";
        case NodeType::EXTRUDE:     return "Extrude";
        case NodeType::SPHERE:      return "Sphere";
        case NodeType::POINT3D:     return "Point (3D)";
        case NodeType::SCRIPT:      return "Script";
        case NodeType::EQUATION:    return "Equation";
        case NodeType::UNION:       return "Union";
        case NodeType::INTERSECTION: return "Intersection";
        case NodeType::DIFFERENCE:  return "Difference";
        case NodeType::OFFSET:      return "Offset";
        case NodeType::CLEARANCE:   return "Clearance";
        case NodeType::SHELL:       return "Shell";
        case NodeType::ATTRACT:     return "Attract";
        case NodeType::REPEL:       return "Repel";
        case NodeType::SCALEX:      return "Scale (X)";
        case NodeType::SCALEY:      return "Scale (Y)";
        case NodeType::SCALEZ:      return "Scale (Z)";
        case NodeType::ROTATEX:     return "Rotate (X)";
        case NodeType::ROTATEY:     return "Rotate (Y)";
        case NodeType::ROTATEZ:     return "Rotate (Z)";
        case NodeType::RECENTER:    return "Re-center";
        case NodeType::TRANSLATE:   return "Translate";
        case NodeType::SLIDER:      return "Slider";
        case NodeType::ITERATE2D:   return "Iterate (2D)";
    }
}
