#include <Python.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QThread>
#include <QGridLayout>
#include <QDesktopWidget>

#include <cmath>

#include "app.h"
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

#include "render/export_mesh.h"
#include "render/export_json.h"
#include "render/export_bitmap.h"

#ifndef GITREV
#warning "Git revision not defined!"
#define GITREV "???"
#endif

App::App(int& argc, char** argv) :
    QApplication(argc, argv),
    graph_scene(new GraphScene()),
    view_scene(new ViewportScene()),
    root(new NodeRoot()), stack(new UndoStack(this))
{
    setGlobalStyle();

    QDesktopWidget desktop;

    auto v = newViewportWindow();
    v->move((desktop.geometry().width() - v->width()) / 2 - 25,
            (desktop.geometry().height() - v->height()) / 2 - 25);

    auto c = newCanvasWindow();
    c->move((desktop.geometry().width() - c->width()) / 2 + 25,
            (desktop.geometry().height() - c->height()) / 2 + 25);

    // When the clean flag on the undo stack changes, update window titles
    connect(stack, &QUndoStack::cleanChanged,
            [&](bool){ emit(windowTitleChanged(getWindowTitle())); });

    connect(view_scene, &ViewportScene::glowChanged,
            graph_scene, &GraphScene::onGlowChange);
    connect(graph_scene, &GraphScene::glowChanged,
            view_scene, &ViewportScene::onGlowChange);
}

App::~App()
{
    graph_scene->deleteLater();
    view_scene->deleteLater();
    root->deleteLater();

    // Prevent segfault-inducing callback during stack destruction
    disconnect(stack, 0, 0, 0);
}

App* App::instance()
{
    return dynamic_cast<App*>(QApplication::instance());
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

    QDataStream out(&file);
    ss.run(&out);

    stack->setClean();
}

void App::onSaveAs()
{
    QString f = QFileDialog::getSaveFileName(NULL, "Save as", "", "*.sb");
    if (!f.isEmpty())
    {
        filename = f;
        emit(windowTitleChanged(getWindowTitle()));
        return onSave();
    }
}

void App::onOpen()
{
    QString f = QFileDialog::getOpenFileName(NULL, "Open", "", "*.sb");
    if (!f.isEmpty())
        loadFile(f);
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

    QDataStream in(&file);
    SceneDeserializer ds(root);
    ds.run(&in);

    if (ds.failed == true)
    {
        QMessageBox::critical(NULL, "Loading error",
                "<b>Loading error:</b><br>" +
                ds.error_message);
        onNew();
    } else {
        makeUI(root);
        graph_scene->setInspectorPositions(ds.inspectors);

        emit(windowTitleChanged(getWindowTitle()));
    }
}

void App::onExportSTL()
{
    Shape s = root->getCombinedShape();
    if (!s.tree)
    {
        QMessageBox::critical(NULL, "Export error",
                "<b>Export error:</b><br>"
                "Cannot export without any shapes in the scene.");
        return;
    }
    if (isinf(s.bounds.xmin) || isinf(s.bounds.xmax) ||
        isinf(s.bounds.ymin) || isinf(s.bounds.ymax) ||
        isinf(s.bounds.zmin) || isinf(s.bounds.zmax))
    {
        QMessageBox::critical(NULL, "Export error",
                "<b>Export error:</b><br>"
                "Some shapes do not have 3D bounds;<br>"
                "cannot export mesh.");
        return;
    }

    auto resolution_dialog = new ResolutionDialog(&s, RESOLUTION_DIALOG_3D);
    if (!resolution_dialog->exec())
        return;

    QString file_name = QFileDialog::getSaveFileName(
            NULL, "Export STL", "", "*.stl");
    if (file_name.isEmpty())
        return;

    auto exporting_dialog = new ExportingDialog();

    auto thread = new QThread();
    auto worker = new ExportMeshWorker(
            s, resolution_dialog->getResolution(),
            file_name);
    delete resolution_dialog;
    worker->moveToThread(thread);

    connect(thread, &QThread::started,
            worker, &ExportMeshWorker::render);
    connect(worker, &ExportMeshWorker::finished,
            thread, &QThread::quit);
    connect(thread, &QThread::finished,
            thread, &QThread::deleteLater);
    connect(thread, &QThread::finished,
            worker, &ExportMeshWorker::deleteLater);
    connect(thread, &QThread::destroyed,
            exporting_dialog, &ExportingDialog::accept);

    thread->start();
    exporting_dialog->exec();
    delete exporting_dialog;
}

void App::onExportHeightmap()
{
    // Verify that we are not mixing 2D and 3D shapes.
    {
        QMap<QString, Shape> shapes = root->getShapes();
        bool has_2d = false;
        bool has_3d = false;
        for (auto s=shapes.begin(); s != shapes.end(); ++s)
            if (isinf(s->bounds.zmin) || isinf(s->bounds.zmax))
                has_2d = true;
            else
                has_3d = true;

        if (has_2d && has_3d)
        {
            QMessageBox::critical(NULL, "Export error",
                    "<b>Export error:</b><br>"
                    "Cannot export with a mix of 2D and 3D shapes in the scene.");
            return;
        }
    }

    Shape s = root->getCombinedShape();
    if (!s.tree)
    {
        QMessageBox::critical(NULL, "Export error",
                "<b>Export error:</b><br>"
                "Cannot export without any shapes in the scene.");
        return;
    }
    if (isinf(s.bounds.xmin) || isinf(s.bounds.xmax) ||
        isinf(s.bounds.ymin) || isinf(s.bounds.ymax))
    {
        QMessageBox::critical(NULL, "Export error",
                "<b>Export error:</b><br>"
                "Some shapes do not have 2D bounds;<br>"
                "cannot export mesh.");
        return;
    }

    // Make a ResolutionDialog for 2D export
    auto resolution_dialog = new ResolutionDialog(&s, RESOLUTION_DIALOG_2D);
    if (!resolution_dialog->exec())
        return;

    QString file_name = QFileDialog::getSaveFileName(
            NULL, "Export .png", "", "*.png");
    if (file_name.isEmpty())
        return;

    auto exporting_dialog = new ExportingDialog();

    auto thread = new QThread();
    auto worker = new ExportBitmapWorker(
            s, resolution_dialog->getResolution(),
            file_name);
    delete resolution_dialog;
    worker->moveToThread(thread);

    connect(thread, &QThread::started,
            worker, &ExportBitmapWorker::render);
    connect(worker, &ExportBitmapWorker::finished,
            thread, &QThread::quit);
    connect(thread, &QThread::finished,
            thread, &QThread::deleteLater);
    connect(thread, &QThread::finished,
            worker, &ExportBitmapWorker::deleteLater);
    connect(thread, &QThread::destroyed,
            exporting_dialog, &ExportingDialog::accept);

    thread->start();
    exporting_dialog->exec();
    delete exporting_dialog;
}

void App::onExportJSON()
{
    QMap<QString, Shape> s = root->getShapes();
    if (s.isEmpty())
    {
        QMessageBox::critical(NULL, "Export error",
                "<b>Export error:</b><br>"
                "Cannot export without any shapes in the scene.");
        return;
    }

    QString file_name = QFileDialog::getSaveFileName(
            NULL, "Export JSON", "", "*.f");
    if (file_name.isEmpty())
        return;

    auto exporting_dialog = new ExportingDialog();

    // Prepare to export (in infix notation)
    auto thread = new QThread();
    auto worker = new ExportJSONWorker(s, file_name, EXPORT_JSON_INFIX);
    worker->moveToThread(thread);

    connect(thread, &QThread::started,
            worker, &ExportJSONWorker::run);
    connect(worker, &ExportJSONWorker::finished,
            thread, &QThread::quit);
    connect(thread, &QThread::finished,
            thread, &QThread::deleteLater);
    connect(thread, &QThread::finished,
            worker, &ExportJSONWorker::deleteLater);
    connect(thread, &QThread::destroyed,
            exporting_dialog, &ExportingDialog::accept);

    thread->start();
    exporting_dialog->exec();
    delete exporting_dialog;
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
    QString t = "antimony [";
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

MainWindow* App::newCanvasWindow()
{
    auto m = new MainWindow();
    m->setCentralWidget(graph_scene->newCanvas());
    m->show();
    return m;
}

MainWindow* App::newViewportWindow()
{
    auto m = new MainWindow();
    m->setCentralWidget(view_scene->newViewport());
    m->show();
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

    top->lockAngle(0, 0);
    front->lockAngle(0, -M_PI/2);
    side->lockAngle(-M_PI/2, -M_PI/2);
    other->spinTo(-M_PI/4, -M_PI/4);

    top->hideViewSelector();
    front->hideViewSelector();
    side->hideViewSelector();
    other->hideViewSelector();

    g->addWidget(top, 0, 0);
    g->addWidget(front, 0, 1);
    g->addWidget(side, 1, 0);
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
    m->resize(400, 600);
    m->show();
    return m;
}

void App::newNode(Node* n)
{
    graph_scene->makeUIfor(n);
    view_scene->makeUIfor(n);
}

void App::makeUI(NodeRoot* r)
{
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
            d->update();
            for (auto k : d->findChildren<Link*>())
            {
                links[d].append(k);
                k->setParent(NULL);
            }
        }
        newNode(n);
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
