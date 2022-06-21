#include <Python.h>

#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <QFileDialog>
#include <QFileOpenEvent>
#include <QJsonDocument>

#include "app/app.h"
#include "app/update.h"

#include "graph/proxy/graph.h"
#include "graph/serialize/serializer.h"
#include "graph/serialize/deserializer.h"

#include "undo/undo_stack.h"

#include "graph/graph.h"

App::App(int& argc, char** argv)
    : QApplication(argc, argv),
      graph(new Graph()), proxy(new GraphProxy(graph)),
      undo_stack(new UndoStack(this))
{
    connect(undo_stack, &QUndoStack::cleanChanged,
            this, &App::cleanChanged);
}

App::~App()
{
    delete graph;
    delete proxy;
    delete undo_stack;
}

App* App::instance()
{
    Q_ASSERT(dynamic_cast<App*>(QApplication::instance()));
    return static_cast<App*>(QApplication::instance());
}

void App::makeDefaultWindows()
{
    newCanvasWindow();
    newViewportWindow();
}

QStringList App::nodePaths() const
{
    QStringList paths;
#if defined Q_OS_MAC
    // On Mac, the 'nodes' folder must be at
    // Antimony.app/Contents/Resources/nodes
    auto path = applicationDirPath().split("/");
    path.removeLast(); // Trim the MacOS folder from the path
    paths << path.join("/") + "/Resources/nodes";
#elif defined Q_OS_LINUX
    // If we're running Antimony from the build folder, use sb/nodes
    paths << applicationDirPath() + "/sb/nodes";
    paths << applicationDirPath() + "/../share/antimony/nodes";
#elif defined Q_OS_WIN32
    // Windows only supports running from the build directory
    paths << applicationDirPath() + "/sb/nodes";
#else
#error "Unknown OS!"
#endif

    for (auto p : QStandardPaths::standardLocations(
            QStandardPaths::AppDataLocation))
    {
        paths << p + "/nodes";
    }

    // Filter paths to canonical forms, keeping only the paths that actually
    // exist as real folders
    QSet<QString> existing_paths;
    for (auto p : paths)
    {
        auto d = QDir(p);
        if (d.exists())
        {
            existing_paths.insert(d.canonicalPath());
        }
    }

    return existing_paths.toList();
}

////////////////////////////////////////////////////////////////////////////////

void App::onNew()
{
    if (undo_stack->isClean() || QMessageBox::question(
                NULL, "Discard unsaved changes?",
                "Discard unsaved changes?") == QMessageBox::Yes)
    {
        graph->clear();
        filename.clear();
        undo_stack->clear();

        emit(filenameChanged(""));
    }
}

void App::onSave()
{
    if (filename.isEmpty())
        return onSaveAs();

    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    {
        auto i = proxy->canvasInfo();
        file.write(QJsonDocument(SceneSerializer::run(graph, &i)).toJson());
    }

    undo_stack->setClean();
}

void App::onSaveAs()
{
    QString f = QFileDialog::getSaveFileName(NULL, "Save as", "", "*.sb");
    if (!f.isEmpty())
    {
#ifdef Q_OS_LINUX
        if (!f.endsWith(".sb"))
            f += ".sb";
#endif
        if (!QFileInfo(QFileInfo(f).path()).isWritable())
        {
            QMessageBox::critical(NULL, "Save As error",
                    "<b>Save As error:</b><br>"
                    "Target file is not writable.");
            return;
        }
        filename = f;
        emit(filenameChanged(filename));
        return onSave();
    }
}

void App::onOpen()
{
    if (undo_stack->isClean() || QMessageBox::question(
                NULL, "Discard unsaved changes?",
                "Discard unsaved changes?") == QMessageBox::Yes)
    {
        QString f = QFileDialog::getOpenFileName(NULL, "Open", "", "*.sb");
        if (!f.isEmpty())
            loadFile(f);
    }
}

void App::onQuit()
{
    if (undo_stack->isClean() || QMessageBox::question(
                NULL, "Discard unsaved changes?",
                "Discard unsaved changes?") == QMessageBox::Yes)
    {
        quit();
    }
}

////////////////////////////////////////////////////////////////////////////////

void App::newCanvasWindow()
{
    proxy->newCanvasWindow();
}

void App::newViewportWindow()
{
    proxy->newViewportWindow();
}

void App::newQuadWindow()
{
    proxy->newQuadWindow();
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
    auto u = new UpdateChecker(this);
    u->start();
}

////////////////////////////////////////////////////////////////////////////////

bool App::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::FileOpen:
            loadFile(static_cast<QFileOpenEvent*>(event)->file());
            return true;
        default:
            return QApplication::event(event);
    }
}

////////////////////////////////////////////////////////////////////////////////

void App::loadFile(QString f)
{
    filename = f;
    graph->clear();

    // XXX disable rendering

    QFile file(f);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(NULL, "Loading error",
                "<b>Loading error:</b><br>"
                "File does not exist.");
        onNew();
        return;
    }

    SceneDeserializer::Info ds;
    const bool success = SceneDeserializer::run(
            QJsonDocument::fromJson(file.readAll()).object(),
            graph, &ds);

    if (!success)
    {
        QMessageBox::critical(NULL, "Loading error",
                "<b>Loading error:</b><br>" +
                ds.error_message);
        onNew();
    } else {
        // If there's a warning message, show it in a box.
        if (!ds.warning_message.isNull())
            QMessageBox::information(NULL, "Loading information",
                    "<b>Loading information:</b><br>" +
                    ds.warning_message);

        proxy->setPositions(ds.frames);
        emit(filenameChanged(filename));
    }
}

////////////////////////////////////////////////////////////////////////////////

void App::onNewWindow()
{
    emit(filenameChanged(filename));
    emit(cleanChanged(undo_stack->isClean()));
}

////////////////////////////////////////////////////////////////////////////////

void App::undo()
{
    undo_stack->undo();
}

void App::redo()
{
    undo_stack->redo();
}

void App::pushUndoStack(UndoCommand* c)
{
    undo_stack->push(c);
}

void App::beginUndoMacro(QString text)
{
    undo_stack->beginMacro(text);
}

void App::endUndoMacro()
{
    undo_stack->endMacro();
}

QAction* App::getUndoAction()
{
    auto a = undo_stack->createUndoAction(this);
    a->setShortcuts(QKeySequence::Undo);
    return a;
}

QAction* App::getRedoAction()
{
    auto a = undo_stack->createRedoAction(this);
    a->setShortcuts(QKeySequence::Redo);
    return a;
}

bool App::isUndoStackClean() const
{
    return undo_stack->isClean();
}
