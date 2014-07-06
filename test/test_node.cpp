#include <Python.h>

#include <QtTest/QtTest>
#include <QDebug>

#include "test_node.h"

#include "datum/name_datum.h"
#include "datum/float_datum.h"
#include "datum/function_datum.h"

#include "node/3d/point3d_node.h"
#include "node/3d/cube_node.h"

#include "node/manager.h"

TestNode::TestNode(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here
}

void TestNode::GetDatum()
{
    Point3D* p = new Point3D("p", "0", "0", "0");
    QVERIFY(p->getDatum<NameDatum>("name"));
    QVERIFY(p->getDatum<NameDatum>("name")->getExpr() == "p");
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
    a->getDatum<NameDatum>("name")->setExpr("new_name");
    QVERIFY(b->getDatum("x")->getValid() == true);
    QVERIFY(a->getDatum("x")->getValue() == b->getDatum("x")->getValue());
    delete a;
    delete b;
}

void TestNode::NewNodeCreation()
{
    Point3D* a = new Point3D("a", "b.x", "0.0", "0.0");
    QVERIFY(a->getDatum("x")->getValid() == false);
    Point3D* b = new Point3D("b", "0.0", "0.0", "0.0");
    QVERIFY(a->getDatum("x")->getValid() == true);
    delete a;
    delete b;
}


void TestNode::DirectRecursiveConnection()
{
    Point3D* a = new Point3D("a", "a.x", "0.0", "0.0");
    QVERIFY(a->getDatum("x")->getValid() == false);
    delete a;
}

void TestNode::LoopingRecursiveConnection()
{
    Point3D* a = new Point3D("a", "a.y", "a.x", "0.0");
    QVERIFY(a->getDatum("x")->getValid() == false);
    QVERIFY(a->getDatum("y")->getValid() == false);
    delete a;
}

void TestNode::ComplexRecursiveConnection()
{
    Point3D* a = new Point3D("a", "0.0", "a.x", "0.0");
    QVERIFY(a->getDatum("x")->getValid() == true);
    QVERIFY(a->getDatum("y")->getValid() == true);

    a->getDatum<FloatDatum>("x")->setExpr("a.y");
    QVERIFY(a->getDatum("x")->getValid() == false);
    QVERIFY(a->getDatum("y")->getValid() == false);

    delete a;
}

void TestNode::ModifyRecursiveConnection()
{
    Point3D* a = new Point3D("a", "0.0", "0.0", "0.0");
    a->getDatum<FloatDatum>("x")->setExpr("a.y");
    a->getDatum<FloatDatum>("y")->setExpr("a.x");
    a->getDatum<FloatDatum>("x")->setExpr("1.0");

    QVERIFY(a->getDatum("x")->getValid() == true);
    QVERIFY(a->getDatum("y")->getValid() == true);
    delete a;
}

void TestNode::TestChildNodes()
{
    CubeNode* c = new CubeNode(0.0, 0.0, 0.0, 1.0);
    ShapeFunctionDatum* d = c->getDatum<ShapeFunctionDatum>("shape");
    QVERIFY(d);
    QVERIFY(d->getValid());
    delete c;
}
