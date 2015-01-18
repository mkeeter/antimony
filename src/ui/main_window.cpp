#include <Python.h>

#include <QKeySequence>
#include <QMouseEvent>
#include <QDebug>
#include <QDirIterator>

#include "app/app.h"

#include "graph/node/node.h"
#include "graph/node/root.h"

#include "ui_main_window.h"
#include "ui/main_window.h"
#include "ui/canvas/canvas.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/viewport/viewport.h"
#include "ui/script/script_pane.h"

#include "control/proxy.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    QActionGroup* view_actions = new QActionGroup(this);
    view_actions->addAction(ui->actionShaded);
    view_actions->addAction(ui->actionHeightmap);
    view_actions->setExclusive(true);

    connectActions(App::instance());
    ui->menuEdit->addAction(App::instance()->undoAction());
    ui->menuEdit->addAction(App::instance()->redoAction());
    setShortcuts();

    populateMenu(ui->menuAdd);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCentralWidget(QWidget* w)
{
    QMainWindow::setCentralWidget(w);

    if (auto c = dynamic_cast<Canvas*>(centralWidget()))
        c->setupUI(ui);
    else if (auto e = dynamic_cast<ScriptPane*>(centralWidget()))
        e->setupUI(ui);
    else
        for (auto v : findChildren<Viewport*>())
            v->setupUI(ui);
}

void MainWindow::connectActions(App* app)
{
    // File menu
    connect(ui->actionSave, &QAction::triggered,
            app, &App::onSave);
    connect(ui->actionSaveAs, &QAction::triggered,
            app, &App::onSaveAs);
    connect(ui->actionNew, &QAction::triggered,
            app, &App::onNew);
    connect(ui->actionOpen, &QAction::triggered,
            app, &App::onOpen);
    connect(ui->actionQuit, &QAction::triggered,
            app, &App::quit);
    connect(ui->actionClose, &QAction::triggered,
            this, &MainWindow::deleteLater);

    // View window
    connect(ui->actionNewCanvas, &QAction::triggered,
            app, &App::newCanvasWindow);
    connect(ui->actionNewViewport, &QAction::triggered,
            app, &App::newViewportWindow);
    connect(ui->actionNewQuad, &QAction::triggered,
            app, &App::newQuadWindow);

    // Export menu
    connect(ui->actionExportMesh, &QAction::triggered,
            app, &App::onExportSTL);
    connect(ui->actionExportHeightmap, &QAction::triggered,
            app, &App::onExportHeightmap);
    connect(ui->actionExportJSON, &QAction::triggered,
            app, &App::onExportJSON);

    // Help menu
    connect(ui->actionAbout, &QAction::triggered,
            app, &App::onAbout);

    // Window title
    setWindowTitle(app->getWindowTitle());
    connect(app, &App::windowTitleChanged,
            this, &MainWindow::setWindowTitle);
}

void MainWindow::setShortcuts()
{
    ui->actionNew->setShortcuts(QKeySequence::New);
    ui->actionOpen->setShortcuts(QKeySequence::Open);
    ui->actionSave->setShortcuts(QKeySequence::Save);
    ui->actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    ui->actionClose->setShortcuts(QKeySequence::Close);
    ui->actionQuit->setShortcuts(QKeySequence::Quit);
    ui->actionCut->setShortcuts(QKeySequence::Cut);
    ui->actionCopy->setShortcuts(QKeySequence::Copy);
    ui->actionPaste->setShortcuts(QKeySequence::Paste);
}

bool MainWindow::isShaded() const
{
    return ui->actionShaded->isChecked();
}

////////////////////////////////////////////////////////////////////////////////

#include "graph/node/nodes/2d.h"
#include "graph/node/nodes/3d.h"
#include "graph/node/nodes/csg.h"
#include "graph/node/nodes/meta.h"
#include "graph/node/nodes/transforms.h"
#include "graph/node/nodes/deform.h"
#include "graph/node/nodes/iterate.h"

#include "graph/datum/types/eval_datum.h"

void MainWindow::createNew(bool recenter, NodeConstructorFunction f,
                           Viewport* v)
{
    v = v ? v : findChild<Viewport*>();
    auto c = findChild<Canvas*>();

    Q_ASSERT((v != NULL) ^ (c != NULL));

    QGraphicsView* view = (v != NULL) ?
        static_cast<QGraphicsView*>(v) :
        static_cast<QGraphicsView*>(c);

    if (recenter)
        QCursor::setPos(view->rect().center());

    if (v)
        v->makeNodeAtCursor(f);
    else if (c)
        c->makeNodeAtCursor(f);
}

void MainWindow::addNodeToMenu(QString category, QString name,
                               QMenu* menu, QMap<QString, QMenu*>* submenus,
                               bool recenter, NodeConstructorFunction f,
                               Viewport* v)
{
    if (!category.isEmpty() && !submenus->contains(category))
        (*submenus)[category] = menu->addMenu(category);
    QAction* a = (category.isEmpty() ? menu : (*submenus)[category])->addAction(name);
    connect(a, &QAction::triggered, [=]{ this->createNew(recenter, f, v); });
}

void MainWindow::populateMenu(QMenu* menu, bool recenter, Viewport* v)
{
    QMap<QString, QMenu*> submenus;

    auto add = [&](QString category, QString name, NodeConstructor constructor)
    {
        addNodeToMenu(category, name, menu, &submenus,
                      recenter, constructor, v);
    };

    add("2D", "Circle", CircleNode);
    add("2D", "Point", Point2DNode);
    add("2D", "Triangle", TriangleNode);
    add("2D", "Rectangle", RectangleNode);
    add("2D", "Text", TextNode);

    add("3D", "Point", Point3DNode);
    add("3D", "Cube", CubeNode);
    add("3D", "Sphere", SphereNode);
    add("3D", "Cylinder", CylinderNode);
    add("3D", "Cone", ConeNode);
    add("3D", "Extrude", ExtrudeNode);

    add("CSG", "Union", UnionNode);
    add("CSG", "Blend", BlendNode);
    add("CSG", "Intersection", IntersectionNode);
    add("CSG", "Difference", DifferenceNode);
    add("CSG", "Offset", OffsetNode);
    add("CSG", "Clearance", ClearanceNode);
    add("CSG", "Shell", ShellNode);

    add("Transform", "Rotate (X)", RotateXNode);
    add("Transform", "Rotate (Y)", RotateYNode);
    add("Transform", "Rotate (Z)", RotateZNode);
    add("Transform", "Reflect (X)", ReflectXNode);
    add("Transform", "Reflect (Y)", ReflectYNode);
    add("Transform", "Reflect (Z)", ReflectZNode);
    add("Transform", "Recenter", RecenterNode);
    add("Transform", "Translate", TranslateNode);

    add("Iterate", "Iterate (2D)", Iterate2DNode);
    add("Iterate", "Iterate (polar)", IteratePolarNode);

    add("Deform", "Attract", AttractNode);
    add("Deform", "Repel", RepelNode);
    add("Deform", "Scale (X)", ScaleXNode);
    add("Deform", "Scale (Y)", ScaleYNode);
    add("Deform", "Scale (Z)", ScaleZNode);

    menu->addSeparator();

    add("", "Script", ScriptNode);

    // Finally, iterate over all of the user-defined scripts.
    QDirIterator itr("nodes", QDirIterator::Subdirectories);
    while (itr.hasNext())
    {
        auto n = itr.next();
        if (!n.endsWith(".node"))
            continue;

        auto split = n.split('/');
        if (split.length() != 3)
            continue;
        QString category = split[1];

        QFile file(n);
        if (!file.open(QIODevice::ReadOnly))
            continue;

        QTextStream in(&file);
        QString txt = in.readAll();

        NodeConstructorFunction constructor =
            [=](float x, float y, float z, float scale, NodeRoot *r)
            {
                auto s = ScriptNode(x, y, z, scale, r);
                static_cast<EvalDatum*>(s->getDatum("_script"))->setExpr(txt);
                return s;
            };
        addNodeToMenu(
                split[1], split[2].replace(".node",""),
                menu, &submenus, recenter, constructor, v);
    }
}
