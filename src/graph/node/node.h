#ifndef NODE_H
#define NODE_H

#include <Python.h>

#include <QObject>
#include <QSet>

#include "graph/node/enum.h"

class Control;
class Datum;
class Link;
class Canvas;
class NodeRoot;

class Node : public QObject
{
    Q_OBJECT
public:
    explicit Node(NodeType::NodeType type, NodeRoot* parent);
    explicit Node(NodeType::NodeType type, QString name, NodeRoot* parent);

    /** Returns a Python proxy that calls getDatum when getattr is called */
    PyObject* proxy();

    /** Looks up a particular Datum by name, return NULL otherwise. */
    Datum* getDatum(QString name) const;

    /** getDatum plus a dynamic cast.
     */
    template <class T>
    T* getDatum(QString name) const
    {
        return dynamic_cast<T*>(getDatum(name));
    }

    /** Returns the NodeType of this node (used in creating Controls)
     */
    NodeType::NodeType getNodeType() const { return type; }

    /** Get this node's name.
     */
    QString getName() const;

    /** Returns a human-readable type name.
     */
    QString getType() const;

    /*
     *  Returns a set of all the links that connect to this node's datums.
     */
    QSet<Link*> getLinks() const;

signals:
    /*
     *  Emitted when the set of datums stored in this Node changes
     *  (usually through ScriptDatum meta-programming magic).
     */
    void datumsChanged();

protected:
    const NodeType::NodeType type;
    Control* control;
};

#endif // NODE_H
