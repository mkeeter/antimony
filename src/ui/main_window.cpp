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
    setShortcuts();

    populateMenu(ui->menuAdd);

    setWindowTitle("antimony");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateMenus()
{
    if (dynamic_cast<Canvas*>(centralWidget()))
    {
        ui->menuView->deleteLater();
    }
    else if (dynamic_cast<ScriptEditor*>(centralWidget()))
    {
        ui->menuView->deleteLater();
        ui->menuAdd->deleteLater();
    }
    else
    {
        for (auto v : findChildren<Viewport*>())
        {
            connect(ui->actionShaded, SIGNAL(triggered()),
                    v->scene, SLOT(invalidate()));
            connect(ui->actionHeightmap, SIGNAL(triggered()),
                    v->scene, SLOT(invalidate()));
        }
    }
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
}

void MainWindow::setShortcuts()
{
    ui->actionNew->setShortcuts(QKeySequence::New);
    ui->actionOpen->setShortcuts(QKeySequence::Open);
    ui->actionSave->setShortcuts(QKeySequence::Save);
    ui->actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    ui->actionClose->setShortcuts(QKeySequence::Close);
    ui->actionQuit->setShortcuts(QKeySequence::Quit);
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

template <Node* (*f)(float, float, float, float, NodeRoot*)>
void MainWindow::createNew(bool recenter, Viewport* v)
{
    v = v ? v : findChild<Viewport*>();
    auto c = findChild<Canvas*>();

    Q_ASSERT((v != NULL) ^ (c != NULL));

    QGraphicsView* view = (v != NULL) ?
        static_cast<QGraphicsView*>(v) :
        static_cast<QGraphicsView*>(c);

    QPoint mouse_pos = recenter
        ? view->rect().center()
        : view->mapFromGlobal(QCursor::pos());
    QPointF scene_pos = view->mapToScene(mouse_pos);

    if (recenter)
        QCursor::setPos(view->mapToGlobal(mouse_pos));

    Node* n;
    if (v)
    {
        QVector3D obj_pos = v->sceneToWorld(scene_pos);
        n = f(obj_pos.x(), obj_pos.y(), obj_pos.z(),
              100 / v->getScale(), App::instance()->getNodeRoot());
    }
    else
    {
        n = f(0, 0, 0, 1, App::instance()->getNodeRoot());
    }

    App::instance()->newNode(n);

    if (v)
    {
        if (auto proxy = v->getControlProxy(n))
        {
            proxy->setClickPos(scene_pos);
            proxy->grabMouse();
        }
    }
    else if (c)
    {
        auto inspector = c->getNodeInspector(n);
        Q_ASSERT(inspector);
        inspector->setSelected(true);
        inspector->setPos(scene_pos);
        inspector->setDragging(true);
        inspector->grabMouse();
    }
}

template <Node* (*f)(float, float, float, float, NodeRoot*)>
void MainWindow::addNodeToMenu(QString category, QString name,
                               QMenu* menu, QMap<QString, QMenu*>* submenus,
                               bool recenter, Viewport* v)
{
    if (submenus && !submenus->contains(category))
        (*submenus)[category] = menu->addMenu(category);
    QAction* a = (submenus ? (*submenus)[category] : menu)->addAction(name);
    connect(a, &QAction::triggered, [=]{ this->createNew<f>(recenter, v); });
}

void MainWindow::populateMenu(QMenu* menu, bool recenter, Viewport* v)
{
    QMap<QString, QMenu*> submenus;

    addNodeToMenu<CircleNode>("2D", "Circle", menu, &submenus, recenter, v);
    addNodeToMenu<Point2DNode>("2D", "Point", menu, &submenus, recenter, v);
    addNodeToMenu<TriangleNode>("2D", "Triangle", menu, &submenus, recenter, v);
    addNodeToMenu<RectangleNode>("2D", "Rectangle", menu, &submenus, recenter, v);
    addNodeToMenu<TextNode>("2D", "Text", menu, &submenus, recenter, v);

    addNodeToMenu<Point3DNode>("3D", "Point", menu, &submenus, recenter, v);
    addNodeToMenu<CubeNode>("3D", "Cube", menu, &submenus, recenter, v);
    addNodeToMenu<SphereNode>("3D", "Sphere", menu, &submenus, recenter, v);
    addNodeToMenu<CylinderNode>("3D", "Cylinder", menu, &submenus, recenter, v);
    addNodeToMenu<ConeNode>("3D", "Cone", menu, &submenus, recenter, v);
    addNodeToMenu<ExtrudeNode>("3D", "Extrude", menu, &submenus, recenter, v);

    addNodeToMenu<UnionNode>("CSG", "Union", menu, &submenus, recenter, v);
    addNodeToMenu<BlendNode>("CSG", "Blend", menu, &submenus, recenter, v);
    addNodeToMenu<IntersectionNode>("CSG", "Intersection", menu, &submenus, recenter, v);
    addNodeToMenu<DifferenceNode>("CSG", "Difference", menu, &submenus, recenter, v);
    addNodeToMenu<OffsetNode>("CSG", "Offset", menu, &submenus, recenter, v);
    addNodeToMenu<ClearanceNode>("CSG", "Clearance", menu, &submenus, recenter, v);
    addNodeToMenu<ShellNode>("CSG", "Shell", menu, &submenus, recenter, v);

    addNodeToMenu<RotateXNode>("Transform", "Rotate (X)", menu, &submenus, recenter, v);
    addNodeToMenu<RotateYNode>("Transform", "Rotate (Y)", menu, &submenus, recenter, v);
    addNodeToMenu<RotateZNode>("Transform", "Rotate (Z)", menu, &submenus, recenter, v);
    addNodeToMenu<ReflectXNode>("Transform", "Reflect (X)", menu, &submenus, recenter, v);
    addNodeToMenu<ReflectYNode>("Transform", "Reflect (Y)", menu, &submenus, recenter, v);
    addNodeToMenu<ReflectZNode>("Transform", "Reflect (Z)", menu, &submenus, recenter, v);
    addNodeToMenu<RecenterNode>("Transform", "Recenter", menu, &submenus, recenter, v);
    addNodeToMenu<TranslateNode>("Transform", "Translate", menu, &submenus, recenter, v);

    addNodeToMenu<Iterate2DNode>("Iterate", "Iterate (2D)", menu, &submenus, recenter, v);

    addNodeToMenu<AttractNode>("Deform", "Attract", menu, &submenus, recenter, v);
    addNodeToMenu<RepelNode>("Deform", "Repel", menu, &submenus, recenter, v);
    addNodeToMenu<ScaleXNode>("Deform", "Scale (X)", menu, &submenus, recenter, v);
    addNodeToMenu<ScaleYNode>("Deform", "Scale (Y)", menu, &submenus, recenter, v);
    addNodeToMenu<ScaleZNode>("Deform", "Scale (Z)", menu, &submenus, recenter, v);

    menu->addSeparator();

    addNodeToMenu<ScriptNode>("", "Script", menu, NULL, recenter, v);
}
