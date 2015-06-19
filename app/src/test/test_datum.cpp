#include <Python.h>

#include <QtTest/QtTest>
#include <QSignalSpy>

#include "test_datum.h"

#include "graph/node/node.h"
#include "graph/node/root.h"

#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/name_datum.h"

TestDatum::TestDatum()
    : r(new NodeRoot()), n(new Node(NodeType::DUMMY, "_dummy", r))
{
    // Nothing to do here
}


void TestDatum::FloatValid()
{
    FloatDatum* d = new FloatDatum("x","12.3", n);
    QVERIFY(d->getValid() == true);
    d->setExpr("1");
    QVERIFY(d->getValid() == true);
    delete d;
}

void TestDatum::FloatInvalid()
{
    FloatDatum* d = new FloatDatum("x","12.3!", n);
    QVERIFY(d->getValid() == false);
    delete d;
}

void TestDatum::FloatValidateFail()
{
    FloatDatum* d = new FloatDatum("x","'this is not a float'", n);
    QVERIFY(d->getValid() == false);
    delete d;
}

void TestDatum::FloatValidToInvalid()
{
    FloatDatum* d = new FloatDatum("x","12.3", n);
    QSignalSpy s(d, SIGNAL(changed()));
    d->setExpr("10.0!");
    QVERIFY(d->getValid() == false);
    QVERIFY(s.count() == 1);
    delete d;
}

void TestDatum::FloatInvalidToValid()
{
    FloatDatum* d = new FloatDatum("x","12.3!", n);
    QSignalSpy s(d, SIGNAL(changed()));
    d->setExpr("10.0");
    QVERIFY(d->getValid() == true);
    QVERIFY(s.count() == 1);
    delete d;
}

void TestDatum::FloatSetSame()
{
    FloatDatum* d = new FloatDatum("x","12.3", n);
    QSignalSpy s(d, SIGNAL(changed()));
    d->setExpr("12.3");
    QVERIFY(d->getValid() == true);
    QVERIFY(s.count() == 0);
    delete d;
}

void TestDatum::SingleInputAccepts()
{
    FloatDatum* a = new FloatDatum("a", "10.1", n);
    FloatDatum* b = new FloatDatum("b", "10.1", n);
    QVERIFY(a->acceptsLink(b->linkFrom()) == true);
    delete a;
    delete b;
}

void TestDatum::SingleInputHasValue()
{
    FloatDatum* a = new FloatDatum("a", "1", n);
    FloatDatum* b = new FloatDatum("b", "2!", n);
    QVERIFY(a->hasInputValue() == false);
    a->addLink(b->linkFrom());
    QVERIFY(a->hasInputValue() == true);
    delete a;
    delete b;
}

void TestDatum::SingleInputLink()
{
    FloatDatum* a = new FloatDatum("a", "1", n);
    FloatDatum* b = new FloatDatum("b", "2!", n);

    QSignalSpy s(a, SIGNAL(changed()));
    a->addLink(b->linkFrom());

    QVERIFY(a->getValid() == false);
    QVERIFY(s.count() == 1);
    delete a;
    delete b;
}

void TestDatum::RecursiveLink()
{
    FloatDatum* a = new FloatDatum("a", "1", n);
    FloatDatum* b = new FloatDatum("a", "1", n);
    a->addLink(b->linkFrom());

    Q_ASSERT(b->acceptsLink(a->linkFrom()) == false);

    delete a;
    delete b;
}

void TestDatum::SingleInputLinkDelete()
{
    FloatDatum* a = new FloatDatum("a", "1", n);
    FloatDatum* b = new FloatDatum("b", "2!", n);
    a->addLink(b->linkFrom());

    QSignalSpy s(a, SIGNAL(changed()));
    delete b;

    QVERIFY(a->hasInputValue() == false);
    QVERIFY(s.count() == 1);
    s.clear();

    b = new FloatDatum("b", "2!", n);
    a->addLink(b->linkFrom());

    QVERIFY(a->hasInputValue() == true);
    QVERIFY(s.count() == 1);
    s.clear();

    a->deleteLink(b);
    QVERIFY(a->hasInputValue() == false);
    QVERIFY(s.count() == 1);
    s.clear();

    delete a;
    delete b;
}

void TestDatum::GetInputDatums()
{
    FloatDatum* a = new FloatDatum("a", "1", n);
    FloatDatum* b = new FloatDatum("b", "2", n);
    Q_ASSERT(a->getInputDatums().length() == 0);
    a->addLink(b->linkFrom());
    Q_ASSERT(a->getInputDatums().length() == 1);
    Q_ASSERT(a->getInputDatums().front() == b);
    a->deleteLink(b);
    Q_ASSERT(a->getInputDatums().length() == 0);

    delete a;
    delete b;

    NameDatum* c = new NameDatum("n", "omg", n);
    Q_ASSERT(c->getInputDatums().length() == 0);
    delete c;
}

void TestDatum::NameValidate()
{
    NameDatum* d;

    d = new NameDatum("a", "hello", n);
    QVERIFY(d->getValid() == true);
    delete d;

    d = new NameDatum("a", "with", n);
    QVERIFY(d->getValid() == false);
    delete d;

    d = new NameDatum("a", "  omgwtf  ", n);
    QVERIFY(d->getValid() == true);
    delete d;

    d = new NameDatum("a", "hi!", n);
    QVERIFY(d->getValid() == false);
    delete d;

    d = new NameDatum("a", "12a", n);
    QVERIFY(d->getValid() == false);
    delete d;

    d = new NameDatum("a", "a12", n);
    QVERIFY(d->getValid() == true);
    delete d;

    d = new NameDatum("a", "a'12", n);
    QVERIFY(d->getValid() == false);
    delete d;
}
