#include <Python.h>

#include <QKeySequence>
#include <QMouseEvent>
#include <QDebug>

#include "app/app.h"

#include "ui_main_window.h"
#include "ui/main_window.h"
#include "ui/canvas/canvas.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/viewport/viewport.h"

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
    } else {
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
#if 0
    connect(ui->actionSave, &QAction::triggered,
            app, &App::onSave);
    connect(ui->actionSaveAs, &QAction::triggered,
            app, &App::onSaveAs);
    connect(ui->actionNew, &QAction::triggered,
            app, &App::onNew);
    connect(ui->actionOpen, &QAction::triggered,
            app, &App::onOpen);
#endif
    connect(ui->actionQuit, &QAction::triggered,
            app, &App::quit);

    // View window
    connect(ui->actionNewCanvas, &QAction::triggered,
            app, &App::newCanvasWindow);
    connect(ui->actionNewViewport, &QAction::triggered,
            app, &App::newViewportWindow);
    connect(ui->actionNewQuad, &QAction::triggered,
            app, &App::newQuadWindow);

    // Export menu
#if 0
    connect(ui->actionExportMesh, &QAction::triggered,
            app, &App::onExportSTL);
    connect(ui->actionExportHeightmap, &QAction::triggered,
            app, &App::onExportHeightmap);
    connect(ui->actionExportJSON, &QAction::triggered,
            app, &App::onExportJSON);
#endif

    // Help menu
    connect(ui->actionAbout, &QAction::triggered,
            app, &App::onAbout);
    connect(ui->actionControls, &QAction::triggered,
            app, &App::onControls);
}

void MainWindow::setShortcuts()
{
    ui->actionNew->setShortcuts(QKeySequence::New);
    ui->actionOpen->setShortcuts(QKeySequence::Open);
    ui->actionSave->setShortcuts(QKeySequence::Save);
    ui->actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    ui->actionQuit->setShortcuts(QKeySequence::Quit);
}

bool MainWindow::isShaded() const
{
    return ui->actionShaded->isChecked();
}

////////////////////////////////////////////////////////////////////////////////

#include "graph/node/node.h"

#include "graph/node/nodes/2d.h"
#include "graph/node/nodes/3d.h"
#include "graph/node/nodes/csg.h"
#include "graph/node/nodes/meta.h"
#include "graph/node/nodes/transforms.h"
#include "graph/node/nodes/deform.h"
#include "graph/node/nodes/variable.h"
#include "graph/node/nodes/iterate.h"

template <Node* (*f)(float, float, float, float, QObject*), bool recenter>
void MainWindow::createNew()
{
    qDebug() << this;

    auto v = findChild<Viewport*>();
    auto c = findChild<Canvas*>();

    qDebug() << v << c;
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
              100 / v->getScale(), NULL);
    }
    else
    {
        n = f(0, 0, 0, 1, NULL);
    }

    App::instance()->newNode(n);

    if (v)
    {
        auto proxy = v->getControlProxy(n);
        Q_ASSERT(proxy);
        proxy->grabMouse();
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

template <Node* (*f)(float, float, float, float, QObject*),
          bool recenter>
void MainWindow::addNodeToMenu(QString category, QString name,
                               QMenu* menu, QMap<QString, QMenu*>* submenus)
{
    if (!submenus->contains(category))
    {
        (*submenus)[category] = menu->addMenu(category);
    }
    QAction* a = (*submenus)[category]->addAction(name);
    connect(a, &QAction::triggered, this, &MainWindow::createNew<f, recenter>);
}

template <bool recenter>
void MainWindow::_populateMenu(QMenu* menu)
{
    QMap<QString, QMenu*> submenus;

    addNodeToMenu<CircleNode, recenter>("2D", "Circle", menu, &submenus);
    addNodeToMenu<Point2DNode, recenter>("2D", "Point", menu, &submenus);
    addNodeToMenu<TriangleNode, recenter>("2D", "Triangle", menu, &submenus);
    addNodeToMenu<RectangleNode, recenter>("2D", "Rectangle", menu, &submenus);
    addNodeToMenu<TextNode, recenter>("2D", "Text", menu, &submenus);

    addNodeToMenu<Point3DNode, recenter>("3D", "Point", menu, &submenus);
    addNodeToMenu<CubeNode, recenter>("3D", "Cube", menu, &submenus);
    addNodeToMenu<SphereNode, recenter>("3D", "Sphere", menu, &submenus);
    addNodeToMenu<CylinderNode, recenter>("3D", "Cylinder", menu, &submenus);
    addNodeToMenu<ConeNode, recenter>("3D", "Cone", menu, &submenus);
    addNodeToMenu<ExtrudeNode, recenter>("3D", "Extrude", menu, &submenus);

    addNodeToMenu<UnionNode, recenter>("CSG", "Union", menu, &submenus);
    addNodeToMenu<BlendNode, recenter>("CSG", "Blend", menu, &submenus);
    addNodeToMenu<IntersectionNode, recenter>("CSG", "Intersection", menu, &submenus);
    addNodeToMenu<DifferenceNode, recenter>("CSG", "Difference", menu, &submenus);
    addNodeToMenu<OffsetNode, recenter>("CSG", "Offset", menu, &submenus);
    addNodeToMenu<ClearanceNode, recenter>("CSG", "Clearance", menu, &submenus);
    addNodeToMenu<ShellNode, recenter>("CSG", "Shell", menu, &submenus);

    addNodeToMenu<RotateXNode, recenter>("Transform", "Rotate (X)", menu, &submenus);
    addNodeToMenu<RotateYNode, recenter>("Transform", "Rotate (Y)", menu, &submenus);
    addNodeToMenu<RotateZNode, recenter>("Transform", "Rotate (Z)", menu, &submenus);
    addNodeToMenu<ReflectXNode, recenter>("Transform", "Reflect (X)", menu, &submenus);
    addNodeToMenu<ReflectYNode, recenter>("Transform", "Reflect (Y)", menu, &submenus);
    addNodeToMenu<ReflectZNode, recenter>("Transform", "Reflect (Z)", menu, &submenus);
    addNodeToMenu<RecenterNode, recenter>("Transform", "Recenter", menu, &submenus);
    addNodeToMenu<TranslateNode, recenter>("Transform", "Translate", menu, &submenus);

    addNodeToMenu<Iterate2DNode, recenter>("Iterate", "Iterate (2D)", menu, &submenus);

    addNodeToMenu<AttractNode, recenter>("Deform", "Attract", menu, &submenus);
    addNodeToMenu<RepelNode, recenter>("Deform", "Repel", menu, &submenus);
    addNodeToMenu<ScaleXNode, recenter>("Deform", "Scale (X)", menu, &submenus);
    addNodeToMenu<ScaleYNode, recenter>("Deform", "Scale (Y)", menu, &submenus);
    addNodeToMenu<ScaleZNode, recenter>("Deform", "Scale (Z)", menu, &submenus);

    addNodeToMenu<SliderNode, recenter>("Variable", "Slider", menu, &submenus);

    addNodeToMenu<ScriptNode, recenter>("Meta", "Script", menu, &submenus);
    addNodeToMenu<EquationNode, recenter>("Meta", "Show equation", menu, &submenus);
}

void MainWindow::populateMenu(QMenu* menu, bool recenter)
{
    if (recenter)
        _populateMenu<true>(menu);
    else
        _populateMenu<false>(menu);
}
