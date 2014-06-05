#include <Python.h>

#include <QApplication>
#include <QDebug>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Py_Initialize();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    int out = a.exec();

    Py_Finalize();
    return out;
}
