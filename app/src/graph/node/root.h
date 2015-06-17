#ifndef NODE_ROOT_H
#define NODE_ROOT_H

#include <Python.h>
#include <QObject>

#include "fab/types/shape.h"

class NameDatum;
class Datum;
class Canvas;
class Node;
class Control;

class NodeRoot : public QObject
{
    Q_OBJECT
public:
    explicit NodeRoot(QObject *parent=NULL);

    /** Returns a Python object suitable for use as a globals dictionary.
     *
     *  This dictionary contains builtins and nodes indexing to proxies,
     *  as well as the built-in math module.
     */
    PyObject* proxyDict(Datum* caller);

    /** Gets an unused name for the given prefix.
     */
    QString getName(QString prefix) const;

    /** Checks to see if the given name is unique.
     */
    bool isNameUnique(QString name) const;

    /** Returns a NameDatum with matching name, or NULL.
     */
    NameDatum* findMatchingName(PyObject* proposed) const;

public slots:
    /** Triggers an update on all EvalDatums that use the new name.
     */
    void onNameChange(QString new_name);
};

#endif // NODE_ROOT_H
