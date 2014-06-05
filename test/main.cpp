#include <Python.h>

#include <QtTest/QtTest>

#include "test_datum.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Py_Initialize();

    TestDatum tc;
    int out = QTest::qExec(&tc, argc, argv);

    Py_Finalize();
    return out;
}

