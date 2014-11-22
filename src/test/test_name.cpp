#include <Python.h>

#include <QtTest/QtTest>
#include <QDebug>

#include "test_name.h"

#include "datum/name_datum.h"

#include "node/node.h"
#include "node/3d.h"

#include "graph/node/manager.h"

TestName::TestName(QObject *parent) :
    QObject(parent)
{
    // Nothing to do here
}

void TestName::NodeName()
{
    Node* p = Point3DNode("p", "0", "0", "0");
    QVERIFY(NodeManager::manager()->getName("p") == "p0");
    delete p;
}

void TestName::NameValid()
{
    Node* p = Point3DNode("p0", "0", "0", "0");
    QVERIFY(p->getDatum("name")->getValid());
    delete p;
}

void TestName::MultiNodeName()
{
    Node* p = Point3DNode("p0", "0", "0", "0");
    QVERIFY(NodeManager::manager()->getName("p") == "p1");
    delete p;
}

void TestName::Rename()
{
    Node* p = Point3DNode("p0", "0", "0", "0");
    p->getDatum<NameDatum>("name")->setExpr("not_p0");
    QVERIFY(NodeManager::manager()->getName("p") == "p0");
    QVERIFY(p->getDatum<NameDatum>("name")->getValid() == true);
    delete p;
}

void TestName::RenameWithSpaces()
{
    Node* p = Point3DNode("p0", "0", "0", "0");
    p->getDatum<NameDatum>("name")->setExpr("   p0   ");
    QVERIFY(p->getDatum<NameDatum>("name")->getValid() == true);
    QVERIFY(NodeManager::manager()->getName("p") == "p1");
    delete p;
}
