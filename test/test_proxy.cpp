#include <Python.h>

#include <QtTest/QtTest>
#include <QSignalSpy>

#include "test_proxy.h"

#include "datum/datum.h"
#include "node/manager.h"
#include "node/3d/point3d.h"

TestProxy::TestProxy(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here
}

void TestProxy::MakeProxy()
{
    Point3D* p = new Point3D("p", "0.0", "1.0", "2.0");
    PyObject* proxy = p->proxy();
    QVERIFY(proxy);
    Py_DECREF(proxy);
    delete p;
}

void TestProxy::GetValidDatum()
{
    Point3D* p = new Point3D("p", "0.0", "1.0", "2.0");
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
    Point3D* p = new Point3D("p", "not a float", "1.0", "2.0");
    PyObject* proxy = p->proxy();
    PyObject* x = PyObject_GetAttrString(proxy, "x");
    QVERIFY(x == NULL);
    Py_DECREF(proxy);
    Py_XDECREF(x);
}

void TestProxy::GetNonexistentDatum()
{
    Point3D* p = new Point3D("p", "0.0", "1.0", "2.0");
    PyObject* proxy = p->proxy();
    PyObject* q = PyObject_GetAttrString(proxy, "q");
    QVERIFY(q == NULL);
    Py_DECREF(proxy);
    Py_XDECREF(q);
    delete p;
}
