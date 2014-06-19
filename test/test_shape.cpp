#include <Python.h>
#include <QTest>

#include "test_shape.h"
#include "datum/shape_datum.h"
#include "datum/output_datum.h"
#include "cpp/shape.h"

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
