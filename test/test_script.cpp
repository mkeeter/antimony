#include <Python.h>

#include <QtTest/QtTest>

#include "test_script.h"
#include "datum/script.h"

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
