#include <Python.h>

#include <QtTest/QtTest>
#include <QSignalSpy>

#include "test_datum.h"
#include "datum/float.h"

void TestDatum::FloatValid()
{
    FloatDatum* d = new FloatDatum("x","12.3");
    QVERIFY(d->getValid() == true);
    delete d;
}

void TestDatum::FloatInvalid()
{
    FloatDatum* d = new FloatDatum("x","12.3!");
    QVERIFY(d->getValid() == false);
    delete d;
}

void TestDatum::FloatValidateFail()
{
    FloatDatum* d = new FloatDatum("x","'this is not a float'");
    QVERIFY(d->getValid() == false);
    delete d;
}

void TestDatum::FloatValidToInvalid()
{
    FloatDatum* d = new FloatDatum("x","12.3");
    QSignalSpy s(d, SIGNAL(changed()));
    d->setExpr("10.0!");
    QVERIFY(d->getValid() == false);
    QVERIFY(s.count() == 1);
    delete d;
}

void TestDatum::FloatInvalidToValid()
{
    FloatDatum* d = new FloatDatum("x","12.3!");
    QSignalSpy s(d, SIGNAL(changed()));
    d->setExpr("10.0");
    QVERIFY(d->getValid() == true);
    QVERIFY(s.count() == 1);
    delete d;
}

void TestDatum::FloatSetSame()
{
    FloatDatum* d = new FloatDatum("x","12.3");
    QSignalSpy s(d, SIGNAL(changed()));
    d->setExpr("12.3");
    QVERIFY(d->getValid() == true);
    QVERIFY(s.count() == 0);
    delete d;
}

void TestDatum::SingleInputAccepts()
{
    FloatDatum* a = new FloatDatum("a", "10.1");
    FloatDatum* b = new FloatDatum("b", "10.1");
    QVERIFY(a->acceptsLink(b->linkFrom()) == true);
    delete a;
    delete b;
}

void TestDatum::SingleInputHasValue()
{
    FloatDatum* a = new FloatDatum("a", "1");
    FloatDatum* b = new FloatDatum("b", "2!");
    QVERIFY(a->hasInputValue() == false);
    a->addLink(b->linkFrom());
    QVERIFY(a->hasInputValue() == true);
    delete a;
    delete b;
}

void TestDatum::SingleInputLink()
{
    FloatDatum* a = new FloatDatum("a", "1");
    FloatDatum* b = new FloatDatum("b", "2!");

    QSignalSpy s(a, SIGNAL(changed()));
    a->addLink(b->linkFrom());

    QVERIFY(a->getValid() == false);
    QVERIFY(s.count() == 1);
    delete a;
    delete b;
}

void TestDatum::SingleInputLinkDelete()
{
    FloatDatum* a = new FloatDatum("a", "1");
    FloatDatum* b = new FloatDatum("b", "2!");
    a->addLink(b->linkFrom());

    QSignalSpy s(a, SIGNAL(changed()));
    delete b;

    QVERIFY(a->hasInputValue() == false);
    QVERIFY(s.count() == 1);
    delete a;
}
