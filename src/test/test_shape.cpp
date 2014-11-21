#include <Python.h>
#include <QTest>
#include <QSignalSpy>

#include "test_shape.h"
#include "datum/shape_datum.h"
#include "datum/output_datum.h"
#include "datum/function_datum.h"
#include "graph/datum/datums/float_datum.h"
#include "datum/link.h"

#include "node/node.h"
#include "node/3d.h"

#include "fab/types/shape.h"

TestShape::TestShape(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here
}

void TestShape::MakeEmptyShape()
{
    ShapeDatum* d = new ShapeDatum("s");
    QVERIFY(d->getValid());
    delete d;
}


void TestShape::ShapeOutput()
{
    ShapeOutputDatum* d;

    d = new ShapeOutputDatum("s");
    QVERIFY(d->getValid() == false);
    delete d;
}


void TestShape::ShapeInput()
{
    Node* p = Point3DNode("p", "0.0", "0.0", "1.0");
    ShapeFunctionDatum* a = new ShapeFunctionDatum("a", p, "circle", {"x","y","z"});

    ShapeDatum* d = new ShapeDatum("d");
    Link* link = a->linkFrom();
    QVERIFY(d->acceptsLink(link));

    d->addLink(link);
    QVERIFY(d->getValid());

    delete p;
    delete d;
}


void TestShape::MultiShapeInput()
{
    Node* p = Point3DNode("p", "0.0", "1.0", "!.0");
    ShapeFunctionDatum* a = new ShapeFunctionDatum("a", p, "circle", {"x","x","y"});
    ShapeFunctionDatum* b = new ShapeFunctionDatum("b", p, "circle", {"x","y","z"});

    ShapeDatum* d = new ShapeDatum("d");
    Link* la = a->linkFrom();
    Link* lb = b->linkFrom();

    d->addLink(la);
    QVERIFY(d->acceptsLink(lb));
    d->addLink(lb);
    QVERIFY(d->getValid() == false);

    QSignalSpy s(d, SIGNAL(changed()));
    p->getDatum<FloatDatum>("z")->setExpr("2.0");
    QVERIFY(d->getValid() == true);
    QCOMPARE(s.count(), 1);

    delete p;
    delete d;
}

void TestShape::DeleteInput()
{
    Node* p = Point3DNode("p", "0.0", "1.0", "!.0");
    ShapeFunctionDatum* a = new ShapeFunctionDatum("a", p, "circle", {"x","y","z"});

    ShapeDatum* d = new ShapeDatum("d");
    Link* link = a->linkFrom();

    d->addLink(link);
    QVERIFY(d->getValid() == false);

    QSignalSpy s(d, SIGNAL(changed()));
    delete link;

    QVERIFY(d->getValid() == true);
    QVERIFY(s.count() == 1);

    delete p;
    delete d;
}
