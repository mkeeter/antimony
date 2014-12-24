#include <Python.h>
#include <QTest>
#include <QSignalSpy>

#include "test_shape.h"
#include "graph/datum/datums/shape_input_datum.h"
#include "graph/datum/datums/shape_output_datum.h"
#include "graph/datum/datums/shape_function_datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/link.h"

#include "graph/node/node.h"
#include "graph/node/root.h"
#include "graph/node/nodes/3d.h"

#include "fab/types/shape.h"

TestShape::TestShape()
    : QObject(), r(new NodeRoot), n(new Node(NodeType::DUMMY, "_dummy", r))
{
    // Nothing to do here
}

void TestShape::MakeEmptyShape()
{
    ShapeInputDatum* d = new ShapeInputDatum("s", n);
    QVERIFY(d->getValid());
    delete d;
}


void TestShape::ShapeOutput()
{
    ShapeOutputDatum* d;

    d = new ShapeOutputDatum("s", n);
    QVERIFY(d->getValid() == false);
    delete d;
}


void TestShape::ShapeInput()
{
    Node* p = Point3DNode("p", "0.0", "0.0", "1.0", r);
    ShapeFunctionDatum* a = new ShapeFunctionDatum("a", p, "circle", {"x","y","z"});

    ShapeInputDatum* d = new ShapeInputDatum("d", n);
    Link* link = a->linkFrom();
    QVERIFY(d->acceptsLink(link));

    d->addLink(link);
    QVERIFY(d->getValid());

    delete p;
    delete d;
}


void TestShape::MultiShapeInput()
{
    Node* p = Point3DNode("p", "0.0", "1.0", "!.0", r);
    ShapeFunctionDatum* a = new ShapeFunctionDatum("a", p, "circle", {"x","x","y"});
    ShapeFunctionDatum* b = new ShapeFunctionDatum("b", p, "circle", {"x","y","z"});

    ShapeInputDatum* d = new ShapeInputDatum("d", n);
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
    Node* p = Point3DNode("p", "0.0", "1.0", "!.0", r);
    ShapeFunctionDatum* a = new ShapeFunctionDatum("a", p, "circle", {"x","y","z"});

    ShapeInputDatum* d = new ShapeInputDatum("d", n);
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
