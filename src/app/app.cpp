#include <Python.h>

#include <QMessageBox>
#include <QFileDialog>
#include <QThread>
#include <QGridLayout>

#include <cmath>

#include "app.h"

#include "ui/dialogs/resolution_dialog.h"
#include "ui/dialogs/exporting_dialog.h"

#include "ui/main_window.h"
#include "ui/canvas/canvas.h"
#include "ui/canvas/scene.h"
#include "ui/viewport/viewport.h"
#include "ui/viewport/scene.h"
#include "ui/script/editor.h"
#include "ui/util/colors.h"

#include "graph/node/node.h"
#include "graph/node/root.h"
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
    root(new NodeRoot())
{
    setGlobalStyle();

    auto v = newViewportWindow();
    v->move(v->pos() - QPoint(25, 25));

    auto c = newCanvasWindow();
    c->move(c->pos() + QPoint(25, 25));
}

App::~App()
{
    graph_scene->deleteLater();
    view_scene->deleteLater();
    root->deleteLater();
}

App* App::instance()
{
    return dynamic_cast<App*>(QApplication::instance());
}

void App::onAbout()
{
    QMessageBox::about(NULL, "Antimony",
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
            "Git revision: " GITREV
            );
}

void App::onNew()
{
    root->deleteLater();
    root = new NodeRoot();
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
}

void App::onSaveAs()
{
    QString f = QFileDialog::getSaveFileName(NULL, "Save as", "", "*.sb");
    if (!f.isEmpty())
    {
        filename = f;
        return onSave();
    }
}

void App::onOpen()
{
    QString f = QFileDialog::getOpenFileName(NULL, "Open", "", "*.sb");
    if (!f.isEmpty())
    {
        filename = f;
        root->deleteLater();
        root = new NodeRoot();
        QFile file(f);
        file.open(QIODevice::ReadOnly);

        QDataStream in(&file);
        SceneDeserializer ds(root);
        ds.run(&in);

        if (ds.failed == true)
        {
            QMessageBox::critical(NULL, "Loading error",
                    "<b>Loading error:</b><br>" +
                    ds.error_message);
        } else {
            for (auto n : root->findChildren<Node*>(
                        "", Qt::FindDirectChildrenOnly))
                newNode(n);

            graph_scene->setInspectorPositions(ds.inspectors);
        }
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
    m->updateMenus();
    m->show();
    return m;
}

MainWindow* App::newViewportWindow()
{
    auto m = new MainWindow();
    m->setCentralWidget(view_scene->newViewport());
    m->updateMenus();
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
    m->updateMenus();
    m->show();
    return m;
}

MainWindow* App::newEditorWindow(ScriptDatum* datum)
{
    auto m = new MainWindow();
    m->setCentralWidget(new ScriptEditor(datum, m));
    m->updateMenus();
    m->resize(400, 600);
    m->show();
    return m;
}

void App::newNode(Node* n)
{
    graph_scene->makeUIfor(n);
    view_scene->makeUIfor(n);
}

Connection* App::newLink(Link* link)
{
    return graph_scene->makeUIfor(link);
}
