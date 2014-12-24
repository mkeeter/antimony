#include <Python.h>

#include <QtTest/QtTest>
#include <QDebug>

#include "test_name.h"

#include "graph/datum/datums/name_datum.h"

#include "graph/node/node.h"
#include "graph/node/root.h"
#include "graph/node/nodes/3d.h"

TestName::TestName() :
    QObject(), r(new NodeRoot(this))
{
    // Nothing to do here
}

void TestName::NodeName()
{
    Node* p = Point3DNode("p", "0", "0", "0", r);
    QVERIFY(r->getName("p") == "p0");
    delete p;
}

void TestName::NameValid()
{
    Node* p = Point3DNode("p0", "0", "0", "0", r);
    QVERIFY(p->getDatum("_name")->getValid());
    delete p;
}

void TestName::MultiNodeName()
{
    Node* p = Point3DNode("p0", "0", "0", "0", r);
    QVERIFY(r->getName("p") == "p1");
    delete p;
}

void TestName::Rename()
{
    Node* p = Point3DNode("p0", "0", "0", "0", r);
    p->getDatum<NameDatum>("_name")->setExpr("not_p0");
    QVERIFY(r->getName("p") == "p0");
    QVERIFY(p->getDatum<NameDatum>("_name")->getValid() == true);
    delete p;
}

void TestName::RenameWithSpaces()
{
    Node* p = Point3DNode("p0", "0", "0", "0", r);
    p->getDatum<NameDatum>("_name")->setExpr("   p0   ");
    QVERIFY(p->getDatum<NameDatum>("_name")->getValid() == true);
    QVERIFY(r->getName("p") == "p1");
    delete p;
}
