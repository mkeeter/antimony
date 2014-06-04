#include <Python.h>

#include <QtTest/QtTest>

#include "test_datum.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTEST_DISABLE_KEYPAD_NAVIGATION
    Py_Initialize();

    TestDatum tc;
    int out = QTest::qExec(&tc, argc, argv);

    Py_Finalize();
    return out;
}

