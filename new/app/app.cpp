#include <Python.h>

#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QUrl>

#include "app/app.h"
#include "window/canvas.h"
#include "graph/proxy/graph.h"

#include "graph/graph.h"

App::App(int& argc, char** argv)
    : QApplication(argc, argv),
      graph(new Graph()), proxy(new GraphProxy(graph, this)),
      update_checker(this)
{
    // Nothing to do here
}

App* App::instance()
{
    Q_ASSERT(dynamic_cast<App*>(QApplication::instance()));
    return static_cast<App*>(QApplication::instance());
}

void App::makeDefaultWindows()
{
    newCanvasWindow();
}

QString App::bundledNodePath() const
{
    auto path = applicationDirPath().split("/");

#if defined Q_OS_MAC
    // On Mac, the 'nodes' folder should be either in
    // Antimony.app/Contents/Resources/nodes (when deployed)
    // or Antimony.app/../sb/nodes (when running from the build directory)
    path.removeLast(); // Trim the MacOS folder from the path

    // When deployed, the nodes folder is in Resources/sb
    if (QDir(path.join("/") + "/Resources/nodes").exists())
    {
        path << "Resources" << "nodes";
    }
    // Otherwise, assume it's at the same level as antimony.app
    else
    {
        for (int i=0; i < 2; ++i)
            path.removeLast();
        path << "sb" << "nodes";
    }
#else
    path << "sb" << "nodes";
#endif

    return path.join("/");
}

QString App::userNodePath() const
{
    auto path = QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation) + "/nodes";
    QDir(path).mkpath(".");
    return path;
}

////////////////////////////////////////////////////////////////////////////////

void App::onNew()
{

}

void App::onSave()
{

}

void App::onSaveAs()
{

}

void App::onOpen()
{

}

void App::onQuit()
{
    quit();
}

////////////////////////////////////////////////////////////////////////////////

void App::newCanvasWindow()
{
    new CanvasWindow(proxy->canvasScene());
}

void App::newViewportWindow()
{

}

void App::newQuadWindow()
{

}

////////////////////////////////////////////////////////////////////////////////

void App::onAbout()
{
    QString txt(
            "<i>Antimony</i><br><br>"
            "CAD from a parallel universe.<br>"
            "<a href=\"https://github.com/mkeeter/antimony\">https://github.com/mkeeter/antimony</a><br><br>"
            "© 2013-2014 Matthew Keeter<br>"
            "<a href=\"mailto:matt.j.keeter@gmail.com\">matt.j.keeter@gmail.com</a><br>"
            "_________________________________________________<br><br>"
            "Includes code from <a href=\"https://github.com/mkeeter/kokopelli\">kokopelli</a>, which is <br>"
            "© 2012-2013 MIT<br>"
            "© 2013-2014 Matthew Keeter<br><br>"
            "Inspired by the <a href=\"http://kokompe.cba.mit.edu\">fab modules</a><br>"
            "_________________________________________________<br><br>"
    );
    QString tag(GITTAG);
    QString branch(GITBRANCH);
    QString rev(GITREV);

    if (!tag.isEmpty())
        txt += "Release: <tt>" + tag + "</tt>";
    else
        txt += "Branch: <tt>" + branch + "</tt>";
    txt += "<br>Git revision: <tt>" + rev + "</tt>";

    QMessageBox::about(NULL, "Antimony", txt);
}

void App::onUpdateCheck()
{
    update_checker.start();
}
