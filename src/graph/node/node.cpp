#include <Python.h>

#include <QStringList>

#include "graph/node/node.h"
#include "graph/node/root.h"
#include "graph/node/proxy.h"

#include "graph/datum/datum.h"
#include "graph/datum/link.h"
#include "graph/datum/datums/name_datum.h"

Node::Node(NodeType::NodeType type, NodeRoot* parent)
    : QObject(parent), type(type), control(NULL), title(getDefaultTitle())
{
    // Nothing to do here
}

Node::Node(NodeType::NodeType type, QString name, NodeRoot* parent)
    : Node(type, parent)
{
    new NameDatum("_name", name, this);
}

void Node::setParent(NodeRoot* root)
{
    QObject::setParent(root);
    for (auto d : findChildren<NameDatum*>(QString(),
                                           Qt::FindDirectChildrenOnly))
        d->update();
}

void Node::setTitle(QString new_title)
{
    if (new_title != title)
    {
        title = new_title;
        emit(titleChanged(title));
    }
}

void Node::updateName()
{
    auto r = dynamic_cast<NodeRoot*>(parent());
    Q_ASSERT(r);

    for (auto d : findChildren<NameDatum*>(QString(),
                                           Qt::FindDirectChildrenOnly))
        d->setExpr(r->getName(d->getExpr() + "_"));
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
    if (auto e = getDatum<EvalDatum>("_name"))
        return e->getExpr();
    return "";
}

QString Node::getDefaultTitle() const
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
        case NodeType::CONE:        return "Cone";
        case NodeType::EXTRUDE:     return "Extrude";
        case NodeType::SPHERE:      return "Sphere";
        case NodeType::POINT3D:     return "Point (3D)";
        case NodeType::SCRIPT:      return "Script";
        case NodeType::UNION:       return "Union";
        case NodeType::BLEND:       return "Blend";
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
        case NodeType::REFLECTX:    return "Reflect (X)";
        case NodeType::REFLECTY:    return "Reflect (Y)";
        case NodeType::REFLECTZ:    return "Reflect (Z)";
        case NodeType::RECENTER:    return "Re-center";
        case NodeType::TRANSLATE:   return "Translate";
        case NodeType::ITERATE2D:   return "Iterate (2D)";
        case NodeType::DUMMY:       return "Dummy";
    }
}

QSet<Link*> Node::getLinks() const
{
    QSet<Link*> links;

    for (auto d : findChildren<Datum*>(QString(), Qt::FindDirectChildrenOnly))
    {
        for (auto k : d->findChildren<Link*>())
            links.insert(k);
        for (auto k : d->inputLinks())
            links.insert(k);
    }
    return links;
}
