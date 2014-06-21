#include <Python.h>

#include <QtTest/QtTest>
#include <QSignalSpy>

#include "test_function.h"
#include "datum/function_datum.h"
#include "datum/float_datum.h"
#include "node/3d/point3d_node.h"

void TestFunction::CircleFunction()
{
    Point3D* p = new Point3D("p", "0.0", "0.0", "1.0");

    ShapeFunctionDatum* d = new ShapeFunctionDatum("c", p, "circle", {"x","y","z"});
    QVERIFY(d->getValid());

    delete p;
}

void TestFunction::InvalidCircle()
{
    Point3D* p = new Point3D("p", "0.0", "0.0", "q.0");

    ShapeFunctionDatum* d1 = new ShapeFunctionDatum("c1", p, "circle", {"x","y","z"});
    QVERIFY(!d1->getValid());

    ShapeFunctionDatum* d2 = new ShapeFunctionDatum("c2", p, "circle", {"x","y","q"});
    QVERIFY(!d2->getValid());

    delete p;
}

void TestFunction::ChangeInput()
{
    Point3D* p = new Point3D("p", "0.0", "0.0", "1.0");

    ShapeFunctionDatum* d = new ShapeFunctionDatum("c", p, "circle", {"x","y","z"});
    QSignalSpy s(d, SIGNAL(changed()));
    dynamic_cast<FloatDatum*>(p->getDatum("x"))->setExpr("1.0");
    QCOMPARE(s.count(), 1);

    delete p;
}


