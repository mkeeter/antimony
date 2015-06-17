#include <Python.h>

#include <QtTest/QtTest>
#include <QSignalSpy>

#include "test_proxy.h"

#include "graph/datum/datum.h"
#include "graph/datum/datums/name_datum.h"

#include "graph/node/node.h"
#include "graph/node/root.h"
#include "graph/node/nodes/3d.h"

TestProxy::TestProxy(QObject* parent)
    : QObject(parent), r(new NodeRoot)
{
    // Nothing to do here
}

void TestProxy::MakeProxy()
{
    Node* p = Point3DNode("p", "0.0", "1.0", "2.0", r);
    PyObject* proxy = p->proxy();
    QVERIFY(proxy);
    Py_DECREF(proxy);
    delete p;
}

void TestProxy::GetValidDatum()
{
    Node* p = Point3DNode("p", "0.0", "1.0", "2.0", r);
    PyObject* proxy = p->proxy();
    PyObject* x = PyObject_GetAttrString(proxy, "x");
    QVERIFY(x == p->getDatum("x")->getValue());
    QVERIFY(x->ob_refcnt == 2);
    Py_DECREF(proxy);
    Py_DECREF(x);
    delete p;
}


void TestProxy::GetInvalidDatum()
{
    Node* p = Point3DNode("p", "not a float", "1.0", "2.0", r);
    PyObject* proxy = p->proxy();
    PyObject* x = PyObject_GetAttrString(proxy, "x");
    QVERIFY(x == NULL);
    PyErr_Clear();
    Py_DECREF(proxy);
    Py_XDECREF(x);
    delete p;
}

void TestProxy::GetNonexistentDatum()
{
    Node* p = Point3DNode("p", "0.0", "1.0", "2.0", r);
    PyObject* proxy = p->proxy();
    PyObject* q = PyObject_GetAttrString(proxy, "q");
    QVERIFY(q == NULL);
    PyErr_Clear();
    Py_DECREF(proxy);
    Py_XDECREF(q);
    delete p;
}

void TestProxy::DatumNameChange()
{
    Node* a = Point3DNode("a", "0.0", "1.0", "2.0", r);
    Node* b = Point3DNode("b", "a.x", "1.0", "2.0", r);
    QVERIFY(b->getDatum("x")->getValid() == true);
    a->getDatum<NameDatum>("_name")->setExpr("q");
    QVERIFY(b->getDatum("x")->getValid() == false);

    delete a;
    delete b;
}
