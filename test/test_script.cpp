#include <Python.h>

#include <QtTest/QtTest>
#include <QSignalSpy>

#include "test_script.h"

#include "datum/script_datum.h"
#include "datum/shape_datum.h"
#include "datum/float_datum.h"
#include "datum/output_datum.h"

#include "node/meta/script_node.h"
#include "node/3d/point3d_node.h"

TestScript::TestScript(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here
}

void TestScript::RunSimpleScript()
{
    ScriptDatum* d = new ScriptDatum("s", "None", NULL);
    QVERIFY(d->getValid() == true);
    delete d;
}

void TestScript::RunInvalidScript()
{
    ScriptDatum* d = new ScriptDatum("s", "y = 12 + x", NULL);
    QVERIFY(d->getValid() == false);
    delete d;
}

void TestScript::RunMultilineScript()
{
    ScriptDatum* d = new ScriptDatum("s", "x = 3\ny = 12 + x", NULL);
    QVERIFY(d->getValid() == true);
    delete d;
}

void TestScript::ImportFabShapes()
{
    ScriptDatum* d = new ScriptDatum("s", "from fab import shapes", NULL);
    QVERIFY(d->getValid() == true);
    delete d;
}

void TestScript::MakeScriptNode()
{
    ScriptNode* n = new ScriptNode("s", "0.0", "0.0", "0.0", "x = 3\ny = 12");
    QVERIFY(n->getDatum("script")->getValid() == true);
    delete n;
}

void TestScript::MakeScriptInput()
{
    ScriptNode* n;

    n = new ScriptNode("s", "0.0", "0.0", "0.0",
                       "input('q', float)");
    QVERIFY(n->getDatum("script")->getValid() == true);
    delete n;


    n = new ScriptNode("s", "0.0", "0.0", "0.0",
                       "input('q', 12)");
    QVERIFY(n->getDatum("script")->getValid() == false);
    delete n;

    n = new ScriptNode("s", "0.0", "0.0", "0.0",
                       "input(12, float)");
    QVERIFY(n->getDatum("script")->getValid() == false);
    delete n;

    n = new ScriptNode("s", "0.0", "0.0", "0.0",
                       "input('q', float, 12)");
    QVERIFY(n->getDatum("script")->getValid() == false);
    delete n;

    n = new ScriptNode("s", "0.0", "0.0", "0.0",
                       "input('q', float, x=12)");
    QVERIFY(n->getDatum("script")->getValid() == false);
    delete n;

    n = new ScriptNode("s", "0.0", "0.0", "0.0",
                       "input('q', str)");
    QVERIFY(n->getDatum("script")->getValid() == false);
    delete n;
}

void TestScript::CheckFloatInput()
{
    ScriptNode* n;
    n = new ScriptNode("s", "0.0", "0.0", "0.0", "input('q', float)");
    QVERIFY(n->getDatum("q") != NULL);
    QVERIFY(n->getDatum<FloatDatum>("q"));
    delete n;
}

void TestScript::ChangeFloatInput()
{
    ScriptNode* n = new ScriptNode("s", "0.0", "0.0", "0.0", "input('q', float); print(q)");

    QSignalSpy s(n->getDatum("script"), SIGNAL(changed()));

    // We change the datum to something invalid, otherwise the script's output
    // will be the same (since it would remain None and valid).
    n->getDatum<FloatDatum>("q")->setExpr("q.0");
    QCOMPARE(s.count(), 1);

    delete n;
}

void TestScript::CheckShapeInput()
{
    ScriptNode* n;
    n = new ScriptNode("s", "0.0", "0.0", "0.0", "from fab import Shape;input('q', Shape)");
    QVERIFY(n->getDatum("q") != NULL);
    QVERIFY(n->getDatum<ShapeDatum>("q"));
    delete n;
}

void TestScript::InvalidInputNames()
{
    ScriptNode* n;
    n = new ScriptNode("s", "0.0", "0.0", "0.0", "from fab import Shape;input('', Shape)");
    QVERIFY(n->getDatum("script")->getValid() == false);
    delete n;

    n = new ScriptNode("s", "0.0", "0.0", "0.0", "from fab import Shape;input('_reserved', Shape)");
    QVERIFY(n->getDatum("script")->getValid() == false);
    QVERIFY(n->getDatum("_reserved") == NULL);

    n = new ScriptNode("s", "0.0", "0.0", "0.0",
                       "from fab import Shape;input('dupe', Shape);input('dupe', Shape)");
    QVERIFY(n->getDatum("script")->getValid() == false);
    QVERIFY(n->getDatum("dupe") != NULL);

    delete n;
}

void TestScript::AddThenRemoveDatum()
{
    ScriptNode* n;
    n = new ScriptNode("s", "0.0", "0.0", "0.0", "input('x', float)");
    ScriptDatum* d = n->getDatum<ScriptDatum>("script");
    QVERIFY(n->getDatum("x") != NULL);
    QVERIFY(n->getDatum<FloatDatum>("x"));

    d->setExpr("from fab import Shape; input('x', Shape)");
    QVERIFY(n->getDatum("x") != NULL);
    QVERIFY(n->getDatum<ShapeDatum>("x"));

    d->setExpr("input('q', float)");
    QVERIFY(n->getDatum("x") == NULL);
    delete n;
}

void TestScript::UseOtherDatum()
{
    Point3D* p = new Point3D("p", "0.!", "0.0", "0.0");
    ScriptNode* n = new ScriptNode("s", "0.0", "0.0", "0.0", "p.x + 1");
    QVERIFY(n->getDatum("script")->getValid() == false);

    QSignalSpy s(n->getDatum("script"), SIGNAL(changed()));
    p->getDatum<FloatDatum>("x")->setExpr("1.0");

    QCOMPARE(s.count(), 1);
    QVERIFY(n->getDatum("script")->getValid() == true);

    delete p;
    delete n;
}

void TestScript::MakeShapeOutput()
{
    ScriptNode* n = new ScriptNode("s", "0.0", "0.0", "0.0", "from fab import shapes; output('q', shapes.circle(0,0,1))");
    QVERIFY(n->getDatum("script")->getValid() == true);
    QVERIFY(n->getDatum("q") != NULL);
    QVERIFY(n->getDatum<ShapeOutputDatum>("q"));
    QVERIFY(n->getDatum("q")->getValid());

    QSignalSpy s(n->getDatum("q"), SIGNAL(changed()));
    QSignalSpy d(n->getDatum("q"), SIGNAL(destroyed()));
    n->getDatum<ScriptDatum>("script")->setExpr("from fab import shapes; output('q', shapes.circle(0,0,2))");
    QCOMPARE(s.count(), 1);
    QCOMPARE(d.count(), 0);

    n->getDatum<ScriptDatum>("script")->setExpr("print('hi there')");
    QVERIFY(n->getDatum("script")->getValid() == true);
    QVERIFY(n->getDatum("q") == NULL);

    delete n;
}

void TestScript::ChangeInputOrder()
{
    ScriptNode* n = new ScriptNode("s", "0.0", "0.0", "0.0", "from fab import Shape; input('a', Shape); input('b', Shape);");
    QVERIFY(n->getDatum("script")->getValid() == true);
    Datum* a = n->getDatum("a");
    Datum* b = n->getDatum("b");

    QVERIFY(a);
    QVERIFY(b);
    QVERIFY(n->findChildren<ShapeDatum*>().front() == a);

    n->getDatum<ScriptDatum>("script")->setExpr("from fab import Shape; input('b', Shape); input('a', Shape);");
    QVERIFY(n->getDatum("a") == a);
    QVERIFY(n->getDatum("b") == b);
    QVERIFY(n->findChildren<ShapeDatum*>().front() == b);

    delete n;
}
