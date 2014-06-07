#include <Python.h>

#include <QtTest/QtTest>
#include <QDebug>

#include "test_node.h"

#include "datum/name.h"
#include "node/3d/point3d.h"
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
