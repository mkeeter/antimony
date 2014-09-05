#include <Python.h>

#include <QApplication>
#include <QDebug>

#include "ui/main_window.h"
#include "app.h"
#include "fab/fab.h"

int main(int argc, char *argv[])
{
    // Create the Application object
    App a(argc, argv);

    // Initialize our fab Python package and the Python interpreter
    fab::preInit();
    Py_Initialize();
    fab::postInit();

    //PyRun_InteractiveLoop(stdin, "<stdin>");

    return a.exec();
}
