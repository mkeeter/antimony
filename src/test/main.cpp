#include <Python.h>

#include <QtTest/QtTest>

#include "fab/fab.h"
#include "graph/hooks/hooks.h"
#include "graph/node/proxy.h"

#include "test_datum.h"
#include "test_node.h"
#include "test_name.h"
#include "test_proxy.h"
#include "test_fab.h"
#include "test_shape.h"
#include "test_script.h"
#include "test_function.h"
#include "test_serialize.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    fab::preInit();
    hooks::preInit();
    proxy::preInit();
    Py_Initialize();

    QString d = QCoreApplication::applicationDirPath();
#if defined Q_OS_MAC
    QStringList path = d.split("/");
    for (int i=0; i < 3; ++i)
        path.removeLast();
    d = path.join("/");
#endif
    fab::postInit(d.toStdString().c_str());

    int out = 0;
    if (!out) {
        TestDatum t;
        out = QTest::qExec(&t, argc, argv);
    }

    if (!out) {
        TestNode t;
        out = QTest::qExec(&t, argc, argv);
    }

    if (!out) {
        TestName t;
        out = QTest::qExec(&t, argc, argv);
    }

    if (!out) {
        TestProxy t;
        out = QTest::qExec(&t, argc, argv);
    }

    if (!out) {
        TestFab t;
        out = QTest::qExec(&t, argc, argv);
    }

    if (!out) {
        TestShape t;
        out = QTest::qExec(&t, argc, argv);
    }

    if (!out) {
        TestScript t;
        out = QTest::qExec(&t, argc, argv);
    }

    if (!out) {
        TestFunction t;
        out = QTest::qExec(&t, argc, argv);
    }

    if (!out) {
        TestSerialize t;
        out = QTest::qExec(&t, argc, argv);
    }

    Py_Finalize();
    return out;
}

