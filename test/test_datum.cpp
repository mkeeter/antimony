#include <Python.h>

#include <QtTest/QtTest>
#include <QSignalSpy>

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
    QSignalSpy s(d, SIGNAL(changed()));
    d->setExpr("10.0!");
    QVERIFY(d->getValid() == false);
    QVERIFY(s.count() == 1);
}

void TestDatum::testFloatInvalidToValid()
{
    FloatDatum* d = new FloatDatum("x","12.3!");
    QSignalSpy s(d, SIGNAL(changed()));
    d->setExpr("10.0");
    QVERIFY(d->getValid() == true);
    QVERIFY(s.count() == 1);
}

void TestDatum::testFloatSetSame()
{
    FloatDatum* d = new FloatDatum("x","12.3");
    QSignalSpy s(d, SIGNAL(changed()));
    d->setExpr("12.3");
    QVERIFY(d->getValid() == true);
    QVERIFY(s.count() == 0);

}
