#ifndef MANAGER_H
#define MANAGER_H

#include <Python.h>
#include <QObject>

class NameDatum;
class Datum;

class NodeManager : public QObject
{
    Q_OBJECT
public:
    explicit NodeManager(QObject *parent = 0);
    PyObject* proxyDict(Datum* caller);
    QString getName(QString prefix) const;

    static NodeManager* manager();

    bool isNameUnique(QString name) const;
    NameDatum* findMatchingName(PyObject* proposed) const;
protected:
    static NodeManager* _manager;
};

#endif // MANAGER_H
