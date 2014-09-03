#include <Python.h>

#include <QtTest/QtTest>

#include "tree/tree.h"
#include "tree/parser.h"
#include "fab/types/shape.h"
#include "fab/fab.h"

#include "test_fab.h"

TestFab::TestFab(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here
}

void TestFab::ImportFab()
{
    QVERIFY(PyRun_SimpleString("import fab") == 0);
}

void TestFab::ParseExpressions()
{
    MathTree* t;

    t = parse("X");
    QVERIFY(t);
    QVERIFY(t->num_levels == 1);
    QVERIFY(t->num_constants == 0);
    free(t);

    t = parse("+Xf1.0");
    QVERIFY(t);
    QVERIFY(t->num_constants == 1);
    QVERIFY(t->num_levels == 2);
    free(t);

    t = parse("*+Xf2.0+Yf2.0");
    QVERIFY(t);
    QVERIFY(t->num_constants == 1);
    QVERIFY(t->num_levels == 3);
    free(t);

    // Partial float
    t = parse("f");
    QVERIFY(t == NULL);
    free(t);

    // Empty string
    t = parse("");
    QVERIFY(t == NULL);
    free(t);

    // Unterminated addition
    t = parse("+X");
    QVERIFY(t == NULL);
    free(t);
}

void TestFab::MoveShape()
{
    Shape a("+Xf1.0");
    // Apply a 2D transform to the shape
    Shape b = a.map(Transform("+Xf2.0","","",""));
    QVERIFY(b.math == "m+Xf2.0  +Xf1.0");
}

void TestFab::MoveBounds()
{
    Bounds a(0, -2, 1, 2);
    // Apply a 2D (inverse) transform to the bounds
    Bounds b = a.map(Transform("","","+Xf2.0","*YY"));
    QVERIFY(b.xmin == 2);
    QVERIFY(b.xmax == 3);
    QVERIFY(b.ymin == -4);
    QVERIFY(b.ymax ==  4);
}

void TestFab::InvalidShape()
{
    try {
        Shape a("+X");
        QVERIFY(false);
    } catch (fab::ParseError e) {
        QVERIFY(true);
    }
}
