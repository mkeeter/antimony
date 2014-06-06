#ifndef NODE_H
#define NODE_H

#include <Python.h>
#include <QObject>

class Control;

class Node : public QObject
{
    Q_OBJECT
public:
    explicit Node(QObject* parent=0);
    PyObject* proxy();
    virtual QString prefix() const=0;

protected:
    Control* control;
};

#endif // NODE_H
