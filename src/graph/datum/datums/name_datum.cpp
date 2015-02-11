#include <Python.h>

#include <QDebug>
#include <QRegExp>

#include "graph/datum/datums/name_datum.h"
#include "graph/datum/input.h"

#include "graph/node/node.h"
#include "graph/node/root.h"

PyObject* NameDatum::kwlist_contains = NULL;

NameDatum::NameDatum(QString name, Node* parent)
    : EvalDatum(name, parent)
{
    connect(this, &NameDatum::changed, this, &NameDatum::onNameChange);
    connect(this, &NameDatum::nameChanged,
            root(), &NodeRoot::onNameChange);
}

NameDatum::NameDatum(QString name, QString expr, Node* parent)
    : NameDatum(name, parent)
{
    setExpr(expr);
}

void NameDatum::onNameChange()
{
    if (getValid())
    {
        auto new_name = getExpr().trimmed();
        if (auto p = dynamic_cast<Node*>(parent()))
        {
            p->setObjectName(getExpr().trimmed());
            for (auto d : p->findChildren<Datum*>(
                        QString(), Qt::FindDirectChildrenOnly))
                emit nameChanged(new_name + "." + d->objectName());
        }
    }
}

QString NameDatum::prepareExpr(QString s) const
{
    return "'" + s.trimmed() + "'";
}

bool NameDatum::validateExpr(QString e) const
{
    QRegExp regex("'[_a-zA-Z][_a-zA-Z0-9]*'");
    return regex.exactMatch(e);
}

bool NameDatum::isKeyword(PyObject* v)
{
    // Lazy initialization of keyword.kwlist.__contains__
    if (!kwlist_contains)
    {
        PyObject* keyword_module = PyImport_ImportModule("keyword");
        PyObject* kwlist = PyObject_GetAttrString(keyword_module, "kwlist");
        Py_DECREF(keyword_module);
        kwlist_contains = PyObject_GetAttrString(kwlist, "__contains__");
        Py_DECREF(kwlist);
    }

    PyObject* args = PyTuple_Pack(1, v);
    PyObject* in_kwlist = PyObject_Call(kwlist_contains, args, NULL);

    Py_DECREF(args);

    bool result = PyObject_IsTrue(in_kwlist);

    Py_DECREF(in_kwlist);

    return result;
}

PyObject* NameDatum::validatePyObject(PyObject *v) const
{
    NameDatum* match = root()->findMatchingName(v);
    if ((match == NULL || match == this) && !isKeyword(v))
    {
        return v;
    }
    else
    {
        Py_DECREF(v);
        return NULL;
    }
}
