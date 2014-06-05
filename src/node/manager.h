#ifndef MANAGER_H
#define MANAGER_H

#include <Python.h>
#include <QObject>

class Datum;

class NodeManager : public QObject
{
    Q_OBJECT
public:
    explicit NodeManager(QObject *parent = 0);
    PyObject* proxyDict(Datum* caller);
    QString getName(QString prefix) const;

    static NodeManager* _manager;
    static NodeManager* manager();
};

#endif // MANAGER_H
