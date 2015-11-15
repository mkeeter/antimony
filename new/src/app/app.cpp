#include <Python.h>

#include <QStandardPaths>
#include <QDir>

#include "app/app.h"
#include "graph/proxy/graph.h"

#include "graph/graph.h"

App::App(int& argc, char** argv)
    : QApplication(argc, argv),
      graph(new Graph()), proxy(new GraphProxy(graph, this))
{
    // Nothing to do here
}

App* App::instance()
{
    Q_ASSERT(dynamic_cast<App*>(QApplication::instance()));
    return static_cast<App*>(QApplication::instance());
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
