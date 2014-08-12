#ifndef NODE_H
#define NODE_H

#include <Python.h>
#include <QObject>

#include "node/node_types.h"

class Control;
class Datum;
class Canvas;

class Node : public QObject
{
    Q_OBJECT
public:
    explicit Node(NodeType::NodeType type, QObject* parent=NULL);
    explicit Node(NodeType::NodeType type, QString name, QObject* parent=NULL);

    /** Returns a Python proxy that calls getDatum when getattr is called */
    PyObject* proxy();

    /** Looks up a particular Datum by name, return NULL otherwise. */
    Datum* getDatum(QString name);

    /** getDatum plus a dynamic cast.
     */
    template <class T>
    T* getDatum(QString name)
    {
        return dynamic_cast<T*>(getDatum(name));
    }

    /** Returns the NodeType of this node (used in creating Controls)
     */
    NodeType::NodeType getNodeType() const { return type; }

protected:
    const NodeType::NodeType type;
    Control* control;
};

#endif // NODE_H
