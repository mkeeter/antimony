#include <Python.h>

#include <QApplication>
#include <QDebug>

#include "mainwindow.h"
#include "fab.h"

int main(int argc, char *argv[])
{
    fab::loadModule();
    Py_Initialize();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    int out = a.exec();

    Py_Finalize();
    return out;
}
