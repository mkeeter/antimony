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
    explicit Node(QObject* parent=0);
    PyObject* proxy();
    virtual QString prefix() const=0;
    Datum* getDatum(QString name);

protected:
    Control* control;
};

#endif // NODE_H
