#include <Python.h>

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QSurfaceFormat>
#include <QTextCodec>

#include <iostream>

#include "app/app.h"
#include "fab/fab.h"
#include "graph/hooks/hooks.h"
#include "graph/node/proxy.h"

int main(int argc, char *argv[])
{
    // Use UTF-8, ignoring any LANG settings in the environment
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    // Set the default OpenGL version to be 2.1 with sample buffers
    QSurfaceFormat format;
    format.setVersion(2, 1);
    QSurfaceFormat::setDefaultFormat(format);

    // Initialize the _fabtypes Python package and the Python interpreter
    fab::preInit();
    hooks::preInit();
    proxy::preInit();
    Py_Initialize();

    // Create the Application object
    App a(argc, argv);

    for (auto arg : a.arguments().mid(1))
        if (arg.startsWith("--"))
        {
            if (arg == "--help")
            {
                std::cout << "Usage: antimony [--help] [--heightmap] [filename]"
                          << std::endl;
                return 0;
            }
            if (arg == "--heightmap")
            {}
            else
            {
                std::cerr << "Invalid command-line argument "
                          << arg.toStdString() << std::endl;
            }
        }

    // Modify Python's default search path to include the application's
    // directory (as this doesn't happen on Linux by default)
    QString d = QCoreApplication::applicationDirPath();
#if defined Q_OS_MAC
    QStringList path = d.split("/");
    for (int i=0; i < 3; ++i)
        path.removeLast();
    d = path.join("/");
#endif
    d += "/sb";
    fab::postInit(d.toStdString().c_str());

    // Check to make sure that the fab module exists
    PyObject* fab = PyImport_ImportModule("fab");
    if (!fab)
    {
        PyErr_Print();
        QMessageBox::critical(NULL, "Import error",
                "Import Error:<br><br>"
                "Could not find <tt>fab</tt> Python module.<br>"
                "Antimony will now exit.");
        exit(1);
    }
    Py_DECREF(fab);

    //PyRun_InteractiveLoop(stdin, "<stdin>");

    if (a.arguments().length() >= 2 && !a.arguments().last().startsWith("--"))
        a.loadFile(a.arguments().last());

    a.makeDefaultWindows();
    return a.exec();
}
