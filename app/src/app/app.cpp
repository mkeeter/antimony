#include <Python.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QThread>
#include <QGridLayout>
#include <QDesktopWidget>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>
#include <QStandardPaths>

#include <cmath>

#include "app/app.h"
#include "app/undo/stack.h"
#include "app/undo/undo_command.h"

#include "ui/dialogs/resolution_dialog.h"
#include "ui/dialogs/exporting_dialog.h"

#include "ui/main_window.h"
#include "ui/canvas/canvas.h"
#include "ui/canvas/graph_scene.h"
#include "ui/viewport/viewport.h"
#include "ui/viewport/viewport_scene.h"
#include "ui/script/script_pane.h"
#include "ui/util/colors.h"

#include "graph/node/node.h"
#include "graph/node/root.h"
#include "graph/datum/datum.h"
#include "graph/datum/link.h"
#include "graph/node/serializer.h"
#include "graph/node/deserializer.h"

#include "fab/types/shape.h"

#ifndef GITREV
#warning "Git revision not defined!"
#define GITREV "???"
#endif

App::App(int& argc, char** argv) :
    QApplication(argc, argv),
    graph_scene(new GraphScene()),
    view_scene(new ViewportScene()),
    root(new NodeRoot()), stack(new UndoStack(this)),
    network(new QNetworkAccessManager(this))
{
    setGlobalStyle();

    // When the clean flag on the undo stack changes, update window titles
    connect(stack, &QUndoStack::cleanChanged,
            [&](bool){ emit(windowTitleChanged(getWindowTitle())); });

    connect(view_scene, &ViewportScene::glowChanged,
            graph_scene, &GraphScene::onGlowChange);
    connect(graph_scene, &GraphScene::glowChanged,
            view_scene, &ViewportScene::onGlowChange);

    connect(network, &QNetworkAccessManager::finished,
            this, &App::onUpdateCheckFinished);
}

App::~App()
{
    graph_scene->deleteLater();
    view_scene->deleteLater();
    root->deleteLater();

    // Prevent segfault-inducing callback during stack destruction
    disconnect(stack, 0, 0, 0);
}

void App::makeDefaultWindows()
{
    QDesktopWidget desktop;

    auto v = newViewportWindow();
    v->move((desktop.geometry().width() - v->width()) / 2 - 25,
            (desktop.geometry().height() - v->height()) / 2 - 25);

    auto c = newCanvasWindow();
    c->move((desktop.geometry().width() - c->width()) / 2 + 25,
            (desktop.geometry().height() - c->height()) / 2 + 25);
}

App* App::instance()
{
    Q_ASSERT(dynamic_cast<App*>(QApplication::instance()));
    return static_cast<App*>(QApplication::instance());
}

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

void App::onNew()
{
    root->deleteLater();
    root = new NodeRoot();

    filename.clear();
    stack->clear();
    emit(windowTitleChanged(getWindowTitle()));
}

void App::onSave()
{
    if (filename.isEmpty())
        return onSaveAs();

    QFile file(filename);
    file.open(QIODevice::WriteOnly);

    SceneSerializer ss(root,
                       graph_scene->inspectorPositions());

    file.write(QJsonDocument(ss.run()).toJson());

    stack->setClean();
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
        emit(windowTitleChanged(getWindowTitle()));
        return onSave();
    }
}

void App::onOpen()
{
    if (stack->isClean() || QMessageBox::question(NULL, "Discard unsaved changes?",
                "Discard unsaved changes?") == QMessageBox::Yes)
    {
        QString f = QFileDialog::getOpenFileName(NULL, "Open", "", "*.sb");
        if (!f.isEmpty())
            loadFile(f);
    }
}

void App::onQuit()
{
    if (stack->isClean() || QMessageBox::question(NULL, "Discard unsaved changes?",
                "Discard unsaved changes?") == QMessageBox::Yes)
        quit();
}

void App::loadFile(QString f)
{
    filename = f;
    root->deleteLater();
    root = new NodeRoot();
    QFile file(f);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(NULL, "Loading error",
                "<b>Loading error:</b><br>"
                "File does not exist.");
        onNew();
        return;
    }

    SceneDeserializer ds(root);
    ds.run(QJsonDocument::fromJson(file.readAll()).object());

    if (ds.failed == true)
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

        makeUI(root);
        graph_scene->setInspectorPositions(ds.inspectors);

        emit(windowTitleChanged(getWindowTitle()));
    }
}

void App::startUpdateCheck()
{
    QNetworkRequest request;
    request.setUrl(QUrl("https://api.github.com/repos/mkeeter/antimony/releases"));
    network->get(request);
}

void App::onUpdateCheckFinished(QNetworkReply* reply)
{
    QRegularExpression ver("(\\d+)\\.(\\d+)\\.(\\d+)([a-z]|)");
    if (!ver.match(GITTAG).hasMatch())
    {
        QMessageBox::critical(NULL, "Update error",
                "<b>Update error:</b><br>"
                "Current build is not tagged to any particular version.");
        return;
    }
    auto current = ver.match(GITTAG).capturedTexts();

    if (reply->error() != QNetworkReply::NoError)
    {
        QMessageBox::critical(NULL, "Update error",
                "<b>Update error:</b><br>"
                "Connection failed.");
        return;
    }

    QJsonParseError err;
    auto out = QJsonDocument::fromJson(reply->readAll(), &err);

    if (err.error != QJsonParseError::NoError)
    {
        QMessageBox::critical(NULL, "Update error",
                "<b>Update error:</b><br>"
                "Could not parse JSON file.");
        return;
    }

    auto latest = out.array()[0].toObject();
    auto update = ver.match(latest["tag_name"].toString()).capturedTexts();

    bool available = false;

    // Check for numerical superiority
    for (int i=1; i < 4; ++i)
        if (current[i].toInt() < update[i].toInt())
            available = true;

    // Check for bug-fix release
    if (!update[4].isEmpty() && (current[4].isEmpty() ||
                current[4] < update[4]))
        available = true;

    if (available)
        QMessageBox::information(NULL, "Update available", QString(
                "<b>Update available:</b><br>"
                "This is version %1<br>"
                "Version "
                "<a href=\"https://github.com/mkeeter/antimony/releases/%2\">"
                "%2</a> is available.").arg(current[0])
                                       .arg(update[0]));
    else
        QMessageBox::information(NULL, "No update available",
                "No update is available at this time.");
}

bool App::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::FileOpen:
            loadFile(static_cast<QFileOpenEvent *>(event)->file());
            return true;
        default:
            return QApplication::event(event);
    }
}

QString App::getWindowTitle() const
{
    QString t = "antimony - %1 - [";
    if (!filename.isEmpty())
        t += filename + "]";
    else
        t += "Untitled]";

    if (!stack->isClean())
        t += "*";
    return t;
}

void App::setGlobalStyle()
{
    setStyleSheet(QString(
            "QToolTip {"
            "   color: %1;"
            "   background-color: %2;"
            "   border: 1px solid %1;"
            "   font-family: Courier"
            "}").arg(Colors::base03.name())
                .arg(Colors::base04.name()));
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

MainWindow* App::newCanvasWindow()
{
    auto m = new MainWindow();
    auto c = graph_scene->newCanvas();
    m->setCentralWidget(c);
    m->show();

    connect(this, &App::jumpToInGraph,
            c, &Canvas::onJumpTo);

    return m;
}

MainWindow* App::newViewportWindow()
{
    auto m = new MainWindow();
    auto v = view_scene->newViewport();
    m->setCentralWidget(v);
    m->show();
    connect(v, &Viewport::jumpTo,
            this, &App::jumpToInGraph);
    connect(this, &App::jumpToInViewport,
            v, &Viewport::onJumpTo);
    return m;
}

MainWindow* App::newQuadWindow()
{
    auto m = new MainWindow();
    auto g = new QGridLayout();

    auto top = view_scene->newViewport();
    auto front = view_scene->newViewport();
    auto side = view_scene->newViewport();
    auto other = view_scene->newViewport();

    for (auto a : {top, front, side})
        for (auto b : {top, front, side})
            if (a != b)
            {
                connect(a, &Viewport::scaleChanged,
                        b, &Viewport::setScale);
                connect(a, &Viewport::centerChanged,
                        b, &Viewport::setCenter);
            }

    for (auto v : {top, front, side, other})
    {
        connect(v, &Viewport::jumpTo,
                this, &App::jumpToInGraph);
        connect(this, &App::jumpToInViewport,
                v, &Viewport::onJumpTo);
    }

    top->lockAngle(0, 0);
    front->lockAngle(0, -M_PI/2);
    side->lockAngle(-M_PI/2, -M_PI/2);
    other->spinTo(-M_PI/4, -M_PI/4);

    top->hideViewSelector();
    front->hideViewSelector();
    side->hideViewSelector();
    other->hideViewSelector();

    g->addWidget(top, 1, 0);
    g->addWidget(front, 0, 0);
    g->addWidget(side, 0, 1);
    g->addWidget(other, 1, 1);
    g->setContentsMargins(0, 0, 0, 0);
    g->setSpacing(2);

    auto w = new QWidget();
    w->setStyleSheet(QString(
                "QWidget {"
                "   background-color: %1;"
                "}").arg(Colors::base01.name()));
    w->setLayout(g);
    m->setCentralWidget(w);
    m->show();
    return m;
}

MainWindow* App::newEditorWindow(ScriptDatum* datum)
{
    auto m = new MainWindow();
    m->setCentralWidget(new ScriptPane(datum, m));
    m->resize(600, 800);
    m->show();
    return m;
}

void App::newNode(Node* n)
{
    graph_scene->makeUIfor(n);
    view_scene->makeRenderWorkersFor(n);
}

void App::makeUI(NodeRoot* r)
{
    QList<Node*> nodes;
    QList<Datum*> datums;
    QMap<Datum*, QList<Link*>> links;

    for (auto n : r->findChildren<Node*>(
                QString(), Qt::FindDirectChildrenOnly))
    {
        n->setParent(root);

        // Save all Links separately
        // (as their UI must be created after all NodeInspectors)
        for (auto d : n->findChildren<Datum*>(
                    QString(), Qt::FindDirectChildrenOnly))
        {
            datums.append(d);
            for (auto k : d->findChildren<Link*>())
            {
                links[d].append(k);
                k->setParent(NULL);
            }
        }
        graph_scene->makeUIfor(n);
        nodes.append(n);
    }

    for (auto i = links.begin(); i != links.end(); ++i)
    {
        for (auto k : i.value())
        {
            k->setParent(i.key());
            k->getTarget()->update();
            newLink(k);
        }
    }

    // Now that all links are created and all nodes are under the same
    // root (in case of address-by-name), run update on every datum.
    for (auto d : datums)
        if (!d->getValid())
            d->update();

    // Finally, make render workers for all of the new nodes.
    // This needs to happen after connections are made; otherwise
    // we end up with a ton of RenderWorkers that all start rendering
    // at once (and eat up all of the threads).
    for (auto n : nodes)
        view_scene->makeRenderWorkersFor(n);
}

Connection* App::newLink(Link* link)
{
    return graph_scene->makeUIfor(link);
}

QAction* App::undoAction()
{
    auto a = stack->createUndoAction(this);
    a->setShortcuts(QKeySequence::Undo);
    return a;
}

QAction* App::redoAction()
{
    auto a = stack->createRedoAction(this);
    a->setShortcuts(QKeySequence::Redo);
    return a;
}

void App::pushStack(UndoCommand* c)
{
    c->setApp(this);
    stack->push(c);
}

void App::undo()
{
    stack->undo();
}

void App::redo()
{
    stack->redo();
}

void App::beginUndoMacro(QString text)
{
    stack->beginMacro(text);
}

void App::endUndoMacro()
{
    stack->endMacro();
}
