#include <Python.h>

#include <QtTest/QtTest>
#include <QDebug>

#include "test_node.h"

#include "datum/name_datum.h"
#include "datum/float_datum.h"
#include "datum/function_datum.h"

#include "node/node.h"
#include "node/3d.h"
#include "node/3d.h"

#include "node/manager.h"

TestNode::TestNode(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here
}

void TestNode::GetDatum()
{
    Node* p = Point3DNode("p", "0", "0", "0");
    QVERIFY(p->getDatum<NameDatum>("name"));
    QVERIFY(p->getDatum<NameDatum>("name")->getExpr() == "p");
    delete p;
}

void TestNode::DeleteNode()
{
    Node* p = Point3DNode("p", "0", "0", "0");
    QSignalSpy s(p->getDatum("x"), SIGNAL(destroyed()));
    delete p;
    QVERIFY(s.count() == 1);
}

void TestNode::EvalValid()
{
    Node* a = Point3DNode("p0", "0.0", "1.0", "2.0");
    QVERIFY(a->getDatum("name")->getValid());
    Node* b = Point3DNode("p1", "p0.x", "1.0", "2.0");
    QVERIFY(b->getDatum("x")->getValid() == true);
    QVERIFY(a->getDatum("x")->getValue() == b->getDatum("x")->getValue());
    delete a;
    delete b;
}

void TestNode::NameChangeEval()
{
    Node* a = Point3DNode("old_name", "0.0", "1.0", "2.0");
    QVERIFY(a->getDatum("name")->getValid());
    Node* b = Point3DNode("p1", "new_name.x", "1.0", "2.0");
    QVERIFY(b->getDatum("x")->getValid() == false);
    a->getDatum<NameDatum>("name")->setExpr("new_name");
    QVERIFY(b->getDatum("x")->getValid() == true);
    QVERIFY(a->getDatum("x")->getValue() == b->getDatum("x")->getValue());
    delete a;
    delete b;
}

void TestNode::NewNodeCreation()
{
    Node* a = Point3DNode("a", "b.x", "0.0", "0.0");
    QVERIFY(a->getDatum("x")->getValid() == false);
    Node* b = Point3DNode("b", "0.0", "0.0", "0.0");
    QVERIFY(a->getDatum("x")->getValid() == true);
    delete a;
    delete b;
}


void TestNode::DirectRecursiveConnection()
{
    Node* a = Point3DNode("a", "a.x", "0.0", "0.0");
    QVERIFY(a->getDatum("x")->getValid() == false);
    delete a;
}

void TestNode::LoopingRecursiveConnection()
{
    Node* a = Point3DNode("a", "a.y", "a.x", "0.0");
    QVERIFY(a->getDatum("x")->getValid() == false);
    QVERIFY(a->getDatum("y")->getValid() == false);
    delete a;
}

void TestNode::ComplexRecursiveConnection()
{
    Node* a = Point3DNode("a", "0.0", "a.x", "0.0");
    QVERIFY(a->getDatum("x")->getValid() == true);
    QVERIFY(a->getDatum("y")->getValid() == true);

    a->getDatum<FloatDatum>("x")->setExpr("a.y");
    QVERIFY(a->getDatum("x")->getValid() == false);
    QVERIFY(a->getDatum("y")->getValid() == false);

    delete a;
}

void TestNode::ModifyRecursiveConnection()
{
    Node* a = Point3DNode("a", "0.0", "0.0", "0.0");
    a->getDatum<FloatDatum>("x")->setExpr("a.y");
    a->getDatum<FloatDatum>("y")->setExpr("a.x");
    a->getDatum<FloatDatum>("x")->setExpr("1.0");

    QVERIFY(a->getDatum("x")->getValid() == true);
    QVERIFY(a->getDatum("y")->getValid() == true);
    delete a;
}

void TestNode::TestChildNodes()
{
    Node* c = CubeNode(0.0, 0.0, 0.0, 1.0);
    ShapeFunctionDatum* d = c->getDatum<ShapeFunctionDatum>("shape");
    QVERIFY(d);
    QVERIFY(d->getValid());
    delete c;
}
