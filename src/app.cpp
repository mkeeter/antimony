#include <Python.h>
#include <QMessageBox>

#include "app.h"
#include "ui_main_window.h"
#include "ui/main_window.h"

#include "node/manager.h"

App::App(int argc, char* argv[]) :
    QApplication(argc, argv), window(new MainWindow)
{
    connectActions();
    window->show();
}

App::~App()
{
    delete window;
    NodeManager::manager()->clear();
}

void App::onAbout()
{
    QMessageBox::about(NULL, "antimony",
            "<b>antimony</b><br><br>"
            "A non-traditional CAD tool.<br>"
            "<a href=\"https://github.com/mkeeter/antimony\">https://github.com/mkeeter/antimony</a><br><br>"
            "© 2013 Matthew Keeter<br>"
            "<a href=\"mailto:matt.j.keeter@gmail.com\">matt.j.keeter@gmail.com</a><br>"
            "_________________________________________________<br><br>"
            "Includes code from <a href=\"https://github.com/mkeeter/kokopelli\">kokopelli</a>, which is <br>"
            "© 2012-2013 Massachusetts Institute of Technology<br>"
            "© 2013 Matthew Keeter<br><br>"
            "Inspired by the <a href=\"http://kokompe.cba.mit.edu\">fab modules</a>"
            );
}

void App::connectActions()
{
    connect(window->ui->actionQuit, SIGNAL(triggered()),
            this, SLOT(quit()));
    connect(window->ui->actionAbout, SIGNAL(triggered()),
            this, SLOT(onAbout()));
}
