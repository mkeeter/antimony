#include <Python.h>

#include <QtTest/QtTest>

#include "test_script.h"

#include "datum/script_datum.h"
#include "datum/shape_datum.h"
#include "datum/float_datum.h"

#include "node/meta/script_node.h"

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
    QVERIFY(dynamic_cast<FloatDatum*>(n->getDatum("q")));
    delete n;
}

void TestScript::CheckShapeInput()
{
    ScriptNode* n;
    n = new ScriptNode("s", "0.0", "0.0", "0.0", "from fab import Shape;input('q', Shape)");
    QVERIFY(n->getDatum("q") != NULL);
    QVERIFY(dynamic_cast<ShapeDatum*>(n->getDatum("q")));
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
    delete n;
}
