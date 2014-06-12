#include <Python.h>

#include <QApplication>
#include <QDebug>

#include "mainwindow.h"
#include "cpp/fab.h"

int main(int argc, char *argv[])
{
    fab::preInit();
    Py_Initialize();
    fab::postInit();
    PyRun_InteractiveLoop(stdin, "<stdin>");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    int out = a.exec();

    Py_Finalize();
    return out;
}
