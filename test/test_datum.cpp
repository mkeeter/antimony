#include <Python.h>
#include <QtTest/QtTest>

#include "test_datum.h"
#include "datum/float.h"

void TestDatum::testFloatValid()
{
    FloatDatum* d = new FloatDatum("x","12.3");
    QVERIFY(d->getValid() == true);
    delete d;
}

void TestDatum::testFloatInvalid()
{
    FloatDatum* d = new FloatDatum("x","12.3!");
    QVERIFY(d->getValid() == false);
    delete d;
}

void TestDatum::testFloatValidToInvalid()
{
    FloatDatum* d = new FloatDatum("x","12.3");
    d->setExpr("10.0!");
    QVERIFY(d->getValid() == false);
}

void TestDatum::testFloatInvalidToValid()
{
    FloatDatum* d = new FloatDatum("x","12.3!");
    d->setExpr("10.0");
    QVERIFY(d->getValid() == true);
}
