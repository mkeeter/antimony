#include <Python.h>

#include <QApplication>
#include <QDebug>

#include "ui/main_window.h"
#include "app.h"
#include "cpp/fab.h"

int main(int argc, char *argv[])
{
    fab::preInit();
    Py_Initialize();
    fab::postInit();
    //PyRun_InteractiveLoop(stdin, "<stdin>");

    int out;
    {
        App a(argc, argv);
        out = a.exec();
    }

    return out;
}
