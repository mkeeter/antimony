#include <Python.h>

#include <QtTest/QtTest>

#include "test_datum.h"
#include "test_node.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Py_Initialize();

    int out = 0;

    if (!out)
    {
        TestDatum t;
        int out = QTest::qExec(&t, argc, argv);
    }

    if (!out)
    {
        TestNode t;
        int out = QTest::qExec(&t, argc, argv);
    }

    Py_Finalize();
    return out;
}

