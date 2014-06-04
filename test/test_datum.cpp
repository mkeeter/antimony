#include <Python.h>
#include <QtTest/QtTest>

#include "test_datum.h"
#include "datum/float.h"

void TestDatum::testFloatEvalValid()
{
    FloatDatum* d = new FloatDatum("x","12.3");
    QVERIFY(d->valid == true);
    delete d;
}

void TestDatum::testFloatEvalInvalid()
{
    FloatDatum* d = new FloatDatum("x","12.3!");
    QVERIFY(d->valid == false);
    delete d;
}
