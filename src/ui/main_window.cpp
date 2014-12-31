#include <Python.h>

#include <QKeySequence>
#include <QMouseEvent>
#include <QDebug>

#include "app/app.h"

#include "graph/node/node.h"
#include "graph/node/root.h"

#include "ui_main_window.h"
#include "ui/main_window.h"
#include "ui/canvas/canvas.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/viewport/viewport.h"
#include "ui/script/editor.h"

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
    else if (auto e = dynamic_cast<ScriptEditor*>(centralWidget()))
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

void MainWindow::createNew(bool recenter, NodeConstructor f, Viewport* v)
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
                               bool recenter, NodeConstructor f, Viewport* v)
{
    if (submenus && !submenus->contains(category))
        (*submenus)[category] = menu->addMenu(category);
    QAction* a = (submenus ? (*submenus)[category] : menu)->addAction(name);
    connect(a, &QAction::triggered, [=]{ this->createNew(recenter, f, v); });
}

void MainWindow::populateMenu(QMenu* menu, bool recenter, Viewport* v)
{
    QMap<QString, QMenu*> submenus;

    addNodeToMenu("2D", "Circle", menu, &submenus, recenter, CircleNode, v);
    addNodeToMenu("2D", "Point", menu, &submenus, recenter, Point2DNode, v);
    addNodeToMenu("2D", "Triangle", menu, &submenus, recenter, TriangleNode, v);
    addNodeToMenu("2D", "Rectangle", menu, &submenus, recenter, RectangleNode, v);
    addNodeToMenu("2D", "Text", menu, &submenus, recenter, TextNode, v);

    addNodeToMenu("3D", "Point", menu, &submenus, recenter, Point3DNode, v);
    addNodeToMenu("3D", "Cube", menu, &submenus, recenter, CubeNode, v);
    addNodeToMenu("3D", "Sphere", menu, &submenus, recenter, SphereNode, v);
    addNodeToMenu("3D", "Cylinder", menu, &submenus, recenter, CylinderNode, v);
    addNodeToMenu("3D", "Cone", menu, &submenus, recenter, ConeNode, v);
    addNodeToMenu("3D", "Extrude", menu, &submenus, recenter, ExtrudeNode, v);

    addNodeToMenu("CSG", "Union", menu, &submenus, recenter, UnionNode, v);
    addNodeToMenu("CSG", "Blend", menu, &submenus, recenter, BlendNode, v);
    addNodeToMenu("CSG", "Intersection", menu, &submenus, recenter, IntersectionNode, v);
    addNodeToMenu("CSG", "Difference", menu, &submenus, recenter, DifferenceNode, v);
    addNodeToMenu("CSG", "Offset", menu, &submenus, recenter, OffsetNode, v);
    addNodeToMenu("CSG", "Clearance", menu, &submenus, recenter, ClearanceNode, v);
    addNodeToMenu("CSG", "Shell", menu, &submenus, recenter, ShellNode, v);

    addNodeToMenu("Transform", "Rotate (X)", menu, &submenus, recenter, RotateXNode, v);
    addNodeToMenu("Transform", "Rotate (Y)", menu, &submenus, recenter, RotateYNode, v);
    addNodeToMenu("Transform", "Rotate (Z)", menu, &submenus, recenter, RotateZNode, v);
    addNodeToMenu("Transform", "Reflect (X)", menu, &submenus, recenter, ReflectXNode, v);
    addNodeToMenu("Transform", "Reflect (Y)", menu, &submenus, recenter, ReflectYNode, v);
    addNodeToMenu("Transform", "Reflect (Z)", menu, &submenus, recenter, ReflectZNode, v);
    addNodeToMenu("Transform", "Recenter", menu, &submenus, recenter, RecenterNode, v);
    addNodeToMenu("Transform", "Translate", menu, &submenus, recenter, TranslateNode, v);

    addNodeToMenu("Iterate", "Iterate (2D)", menu, &submenus, recenter, Iterate2DNode, v);

    addNodeToMenu("Deform", "Attract", menu, &submenus, recenter, AttractNode, v);
    addNodeToMenu("Deform", "Repel", menu, &submenus, recenter, RepelNode, v);
    addNodeToMenu("Deform", "Scale (X)", menu, &submenus, recenter, ScaleXNode, v);
    addNodeToMenu("Deform", "Scale (Y)", menu, &submenus, recenter, ScaleYNode, v);
    addNodeToMenu("Deform", "Scale (Z)", menu, &submenus, recenter, ScaleZNode, v);

    menu->addSeparator();

    addNodeToMenu("", "Script", menu, NULL, recenter, ScriptNode, v);
}
