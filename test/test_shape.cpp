#include <Python.h>
#include <QTest>

#include "test_shape.h"
#include "datum/shape_datum.h"

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
