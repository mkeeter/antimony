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
    explicit Node(QObject* parent=NULL);
    explicit Node(QString name, QObject* parent=NULL);

    /** Returns a Python proxy that calls getDatum when getattr is called */
    PyObject* proxy();

    /** Looks up a particular Datum by name, return NULL otherwise. */
    Datum* getDatum(QString name);

    /** Returns the NodeType, which is set as a templated parameter
     *  in a virtual function override.
     */
    virtual NodeType::NodeType getType() const=0;

    /** getDatum plus a dynamic cast.
     */
    template <class T>
    T* getDatum(QString name)
    {
        return dynamic_cast<T*>(getDatum(name));
    }

protected:
    Control* control;
};

#endif // NODE_H
