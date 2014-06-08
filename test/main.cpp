#include <boost/python.hpp>

#include <QtTest/QtTest>

#include "test_datum.h"
#include "test_node.h"
#include "test_name.h"
#include "test_proxy.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Py_Initialize();

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

    Py_Finalize();
    return out;
}

