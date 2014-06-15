#include <Python.h>

#include <QtTest/QtTest>
#include <QDebug>

#include "test_node.h"

#include "datum/name_datum.h"
#include "node/3d/point3d_node.h"
#include "node/manager.h"

TestNode::TestNode(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here
}

void TestNode::GetDatum()
{
    Point3D* p = new Point3D("p", "0", "0", "0");
    QVERIFY(dynamic_cast<NameDatum*>(p->getDatum("name")));
    QVERIFY(dynamic_cast<NameDatum*>(p->getDatum("name"))->getExpr() == "p");
    delete p;
}

void TestNode::DeleteNode()
{
    Point3D* p = new Point3D("p", "0", "0", "0");
    QSignalSpy s(p->getDatum("x"), SIGNAL(destroyed()));
    delete p;
    QVERIFY(s.count() == 1);
}

void TestNode::EvalValid()
{
    Point3D* a = new Point3D("p0", "0.0", "1.0", "2.0");
    QVERIFY(a->getDatum("name")->getValid());
    Point3D* b = new Point3D("p1", "p0.x", "1.0", "2.0");
    QVERIFY(b->getDatum("x")->getValid() == true);
    QVERIFY(a->getDatum("x")->getValue() == b->getDatum("x")->getValue());
    delete a;
    delete b;
}

void TestNode::NameChangeEval()
{
    Point3D* a = new Point3D("old_name", "0.0", "1.0", "2.0");
    QVERIFY(a->getDatum("name")->getValid());
    Point3D* b = new Point3D("p1", "new_name.x", "1.0", "2.0");
    QVERIFY(b->getDatum("x")->getValid() == false);
    dynamic_cast<NameDatum*>(a->getDatum("name"))->setExpr("new_name");
    QVERIFY(b->getDatum("x")->getValid() == true);
    QVERIFY(a->getDatum("x")->getValue() == b->getDatum("x")->getValue());
    delete a;
    delete b;
}

