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
    PyObject* proxy();
    Datum* getDatum(QString name);

protected:
    Control* control;
};

#endif // NODE_H
