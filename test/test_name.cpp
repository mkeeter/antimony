#include <Python.h>

#include <QtTest/QtTest>
#include <QDebug>

#include "test_name.h"

#include "datum/name.h"
#include "node/3d/point3d.h"
#include "node/manager.h"

TestName::TestName(QObject *parent) :
    QObject(parent)
{
    // Nothing to do here
}

void TestName::NodeName()
{
    Point3D* p = new Point3D("p", "0", "0", "0");
    QVERIFY(NodeManager::manager()->getName("p") == "p0");
    delete p;
}

void TestName::MultiNodeName()
{
    Point3D* p = new Point3D("p0", "0", "0", "0");
    QVERIFY(NodeManager::manager()->getName("p") == "p1");
    delete p;
}

void TestName::Rename()
{
    Point3D* p = new Point3D("p0", "0", "0", "0");
    dynamic_cast<NameDatum*>(p->getDatum("name"))->setExpr("not_p0");
    QVERIFY(NodeManager::manager()->getName("p") == "p0");
    QVERIFY(dynamic_cast<NameDatum*>(p->getDatum("name"))->getValid() == true);
    delete p;
}

void TestName::RenameWithSpaces()
{
    Point3D* p = new Point3D("p0", "0", "0", "0");
    dynamic_cast<NameDatum*>(p->getDatum("name"))->setExpr("   p0   ");
    QVERIFY(dynamic_cast<NameDatum*>(p->getDatum("name"))->getValid() == true);
    QVERIFY(NodeManager::manager()->getName("p") == "p1");
    delete p;
}
