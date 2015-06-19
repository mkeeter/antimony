#include <Python.h>

#include <QtTest/QtTest>
#include <QSignalSpy>

#include "test_script.h"

#include "graph/datum/datums/script_datum.h"
#include "graph/datum/datums/shape_input_datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/shape_output_datum.h"

#include "graph/node/node.h"
#include "graph/node/root.h"
#include "graph/node/nodes/meta.h"
#include "graph/node/nodes/3d.h"

TestScript::TestScript(QObject* parent)
    : QObject(parent), r(new NodeRoot)
{
    // Nothing to do here
}

void TestScript::RunSimpleScript()
{
    Node* n = new Node(NodeType::DUMMY, "_dummy", r);
    ScriptDatum* d = new ScriptDatum("s", "None", n);
    QVERIFY(d->getValid() == true);
    delete d;
    delete n;
}

void TestScript::RunInvalidScript()
{
    Node* n = new Node(NodeType::DUMMY, "_dummy", r);
    ScriptDatum* d = new ScriptDatum("s", "y = 12 + x", n);
    QVERIFY(d->getValid() == false);
    delete d;
    delete n;
}

void TestScript::RunMultilineScript()
{
    Node* n = new Node(NodeType::DUMMY, "_dummy", r);
    ScriptDatum* d = new ScriptDatum("s", "x = 3\ny = 12 + x", n);
    QVERIFY(d->getValid() == true);
    delete d;
    delete n;
}

void TestScript::ImportFabShapes()
{
    Node* n = new Node(NodeType::DUMMY, "_dummy", r);
    ScriptDatum* d = new ScriptDatum("s", "from fab import shapes", n);
    QVERIFY(d->getValid() == true);
    delete d;
    delete n;
}

void TestScript::MakeScriptNode()
{
    Node* n = ScriptNode("s", "0.0", "0.0", "0.0", "x = 3\ny = 12", r);
    QVERIFY(n->getDatum("_script")->getValid() == true);
    delete n;
}

void TestScript::MakeScriptInput()
{
    Node* n;

    n = ScriptNode("s", "0.0", "0.0", "0.0",
                       "input('q', float)", r);
    QVERIFY(n->getDatum("_script")->getValid() == true);
    delete n;


    n = ScriptNode("s", "0.0", "0.0", "0.0",
                       "input('q', 12)", r);
    QVERIFY(n->getDatum("_script")->getValid() == false);
    delete n;

    n = ScriptNode("s", "0.0", "0.0", "0.0",
                       "input(12, float)", r);
    QVERIFY(n->getDatum("_script")->getValid() == false);
    delete n;

    n = ScriptNode("s", "0.0", "0.0", "0.0",
                       "input('q', float, 12)", r);
    QVERIFY(n->getDatum("_script")->getValid() == true);
    delete n;

    n = ScriptNode("s", "0.0", "0.0", "0.0",
                       "input('q', float, x=12)", r);
    QVERIFY(n->getDatum("_script")->getValid() == false);
    delete n;

    n = ScriptNode("s", "0.0", "0.0", "0.0",
                       "input('q', str)", r);
    QVERIFY(n->getDatum("_script")->getValid() == true);
    delete n;
}

void TestScript::CheckFloatInput()
{
    Node* n;
    n = ScriptNode("s", "0.0", "0.0", "0.0", "input('q', float)", r);
    QVERIFY(n->getDatum("q") != NULL);
    QVERIFY(n->getDatum<FloatDatum>("q"));
    delete n;
}

void TestScript::ChangeFloatInput()
{
    Node* n = ScriptNode("s", "0.0", "0.0", "0.0", "input('q', float); print(q)", r);

    QSignalSpy s(n->getDatum("_script"), SIGNAL(changed()));

    // We change the datum to something invalid, otherwise the script's output
    // will be the same (since it would remain None and valid).
    n->getDatum<FloatDatum>("q")->setExpr("q.0");
    QCOMPARE(s.count(), 1);

    delete n;
}

void TestScript::CheckShapeInput()
{
    Node* n;
    n = ScriptNode("s", "0.0", "0.0", "0.0", "from fab.types import Shape;input('q', Shape)", r);
    QVERIFY(n->getDatum("q") != NULL);
    QVERIFY(n->getDatum<ShapeInputDatum>("q"));
    delete n;
}

void TestScript::InvalidInputNames()
{
    Node* n;
    n = ScriptNode("s", "0.0", "0.0", "0.0", "from fab.types import Shape;input('', Shape)", r);
    QVERIFY(n->getDatum("_script")->getValid() == false);
    delete n;

    n = ScriptNode("s", "0.0", "0.0", "0.0", "from fab.types import Shape;input('_reserved', Shape)", r);
    QVERIFY(n->getDatum("_script")->getValid() == false);
    QVERIFY(n->getDatum("_reserved") == NULL);
    delete n;

    n = ScriptNode("s", "0.0", "0.0", "0.0",
                       "from fab.types import Shape;input('dupe', Shape);input('dupe', Shape)", r);
    QVERIFY(n->getDatum("_script")->getValid() == false);
    QVERIFY(n->getDatum("dupe") != NULL);
    delete n;
}

void TestScript::AddThenRemoveDatum()
{
    Node* n;
    n = ScriptNode("s", "0.0", "0.0", "0.0", "input('x', float)", r);
    ScriptDatum* d = n->getDatum<ScriptDatum>("_script");
    QVERIFY(n->getDatum("x") != NULL);
    QVERIFY(n->getDatum<FloatDatum>("x"));

    d->setExpr("from fab.types import Shape; input('x', Shape)");
    QVERIFY(n->getDatum("x") != NULL);
    QVERIFY(n->getDatum<ShapeInputDatum>("x"));

    d->setExpr("input('q', float)");
    QVERIFY(n->getDatum("x") == NULL);
    delete n;
}

void TestScript::UseOtherDatum()
{
    Node* p = Point3DNode("p", "0.!", "0.0", "0.0", r);
    Node* n = ScriptNode("s", "0.0", "0.0", "0.0", "p.x + 1", r);
    QVERIFY(n->getDatum("_script")->getValid() == false);

    QSignalSpy s(n->getDatum("_script"), SIGNAL(changed()));
    p->getDatum<FloatDatum>("x")->setExpr("1.0");

    QCOMPARE(s.count(), 1);
    QVERIFY(n->getDatum("_script")->getValid() == true);

    delete p;
    delete n;
}

void TestScript::MakeShapeOutput()
{
    Node* n = ScriptNode("s", "0.0", "0.0", "0.0", "from fab import shapes; output('q', shapes.circle(0,0,1))", r);
    QVERIFY(n->getDatum("_script")->getValid() == true);
    QVERIFY(n->getDatum("q") != NULL);
    QVERIFY(n->getDatum<ShapeOutputDatum>("q"));
    QVERIFY(n->getDatum("q")->getValid());

    QSignalSpy s(n->getDatum("q"), SIGNAL(changed()));
    QSignalSpy d(n->getDatum("q"), SIGNAL(destroyed()));
    n->getDatum<ScriptDatum>("_script")->setExpr("from fab import shapes; output('q', shapes.circle(0,0,2))");
    QCOMPARE(s.count(), 1);
    QCOMPARE(d.count(), 0);

    n->getDatum<ScriptDatum>("_script")->setExpr("print('hi there')");
    QVERIFY(n->getDatum("_script")->getValid() == true);
    QVERIFY(n->getDatum("q") == NULL);

    delete n;
}

void TestScript::ChangeInputOrder()
{
    Node* n = ScriptNode("s", "0.0", "0.0", "0.0", "from fab.types import Shape; input('a', Shape); input('b', Shape);", r);
    QVERIFY(n->getDatum("_script")->getValid() == true);
    Datum* a = n->getDatum("a");
    Datum* b = n->getDatum("b");

    QVERIFY(a);
    QVERIFY(b);
    QVERIFY(n->findChildren<ShapeInputDatum*>().front() == a);

    n->getDatum<ScriptDatum>("_script")->setExpr("from fab.types import Shape; input('b', Shape); input('a', Shape);");
    QVERIFY(n->getDatum("a") == a);
    QVERIFY(n->getDatum("b") == b);
    QVERIFY(n->findChildren<ShapeInputDatum*>().front() == b);

    delete n;
}

void TestScript::ShapeUpdate()
{
    Node* n = ScriptNode("s", "0.0", "0.0", "1.0", "from fab import shapes; output('q', shapes.circle(0, 0, s._z))", r);
    QVERIFY(n->getDatum("_script")->getValid() == false);
    new FloatDatum("_z", "1.0", n);
    QVERIFY(n->getDatum("_script")->getValid() == true);

    Datum* out = n->getDatum("q");
    QVERIFY(out);
    QVERIFY(out->getValid());
    QSignalSpy outSpy(out, SIGNAL(changed()));
    QSignalSpy floatSpy(n->getDatum("_z"), SIGNAL(changed()));

    dynamic_cast<FloatDatum*>(n->getDatum("_z"))->setExpr("2.0");
    QCOMPARE(floatSpy.count(), 1);
    QCOMPARE(outSpy.count(), 1);

    delete n;
}
