#ifndef NODE_H
#define NODE_H

#include <Python.h>
#include <QObject>

class Control;
class Datum;

class Node : public QObject
{
    Q_OBJECT
public:
    explicit Node(QString name, QObject* parent=NULL);

    /** Returns a Python proxy that calls getDatum when getattr is called */
    PyObject* proxy();

    /** Looks up a particular Datum by name, return NULL otherwise. */
    Datum* getDatum(QString name);

protected:
    Control* control;
};

#endif // NODE_H
