#include <Python.h>

#include <QApplication>
#include <QDebug>

#include "ui/main_window.h"
#include "app.h"
#include "cpp/fab.h"
#include "node/manager.h"

int main(int argc, char *argv[])
{
    fab::preInit();
    Py_Initialize();
    fab::postInit();
    //PyRun_InteractiveLoop(stdin, "<stdin>");

    App a(argc, argv);
    int out = a.exec();
    NodeManager::manager()->clear();

    Py_Finalize();
    return out;
}
