#include <Python.h>

#include <QKeySequence>

#include "ui_main_window.h"

#include "ui/main_window.h"
#include "ui/canvas.h"
#include "ui/script.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    addCanvas();
    addScriptEditor();
    populateMenu(ui->menuAdd);

    // Clear the info label for now.
    ui->label->setText("");

    setWindowTitle("antimony");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addCanvas()
{
    canvas = new Canvas(this);
    ui->gridLayout->addWidget(canvas, 0, 0);
    canvas->lower();
}

void MainWindow::addScriptEditor()
{
    script = new ScriptEditor(this);
    ui->gridLayout->addWidget(script, 0, 0);
}

void MainWindow::setShortcuts()
{
    ui->actionNew->setShortcuts(QKeySequence::New);
    ui->actionOpen->setShortcuts(QKeySequence::Open);
    ui->actionSave->setShortcuts(QKeySequence::Save);
    ui->actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    ui->actionQuit->setShortcuts(QKeySequence::Quit);
}

void MainWindow::openScript(ScriptDatum *d)
{
    script->setDatum(d);
}

////////////////////////////////////////////////////////////////////////////////

#include "node/node.h"
#include "node/manager.h"
#include "control/control.h"

#include "node/2d.h"
#include "node/3d.h"
#include "node/csg.h"
#include "node/meta.h"
#include "node/transforms.h"
#include "node/deform.h"
#include "node/variable.h"
#include "node/iterate.h"

template <Node* (*f)(float, float, float, float, QObject*)>
void MainWindow::createNew()
{
    QPoint mouse_pos  = canvas->rect().center();
    QPointF scene_pos = canvas->sceneRect().center();
    QVector3D obj_pos = canvas->sceneToWorld(scene_pos);

    QCursor::setPos(canvas->mapToGlobal(mouse_pos));

    Node* n = f(obj_pos.x(), obj_pos.y(), obj_pos.z(),
                100 / canvas->getScale(), NULL);
    Control* c = Control::makeControlFor(canvas, n);
    c->grabMouse();
    c->setClickPos(scene_pos);
}

template <Node* (*f)(float, float, float, float, QObject*)>
void MainWindow::addNodeToMenu(QString category, QString name,
                               QMenu* menu, QMap<QString, QMenu*>* submenus)
{
    if (!submenus->contains(category))
    {
        (*submenus)[category] = menu->addMenu(category);
    }
    QAction* a = (*submenus)[category]->addAction(name);
    connect(a, &QAction::triggered, this, &MainWindow::createNew<f>);
}

void MainWindow::populateMenu(QMenu* menu)
{
    QMap<QString, QMenu*> submenus;
    addNodeToMenu<Point3DNode>("3D", "Point", menu, &submenus);
    addNodeToMenu<CubeNode>("3D", "Cube", menu, &submenus);
    addNodeToMenu<SphereNode>("3D", "Sphere", menu, &submenus);
    addNodeToMenu<CylinderNode>("3D", "Cylinder", menu, &submenus);
    addNodeToMenu<ExtrudeNode>("3D", "Extrude", menu, &submenus);
    addNodeToMenu<CircleNode>("2D", "Circle", menu, &submenus);
    addNodeToMenu<Point2DNode>("2D", "Point", menu, &submenus);
    addNodeToMenu<TriangleNode>("2D", "Triangle", menu, &submenus);
    addNodeToMenu<TextNode>("2D", "Text", menu, &submenus);
    addNodeToMenu<UnionNode>("CSG", "Union", menu, &submenus);
    addNodeToMenu<IntersectionNode>("CSG", "Intersection", menu, &submenus);
    addNodeToMenu<DifferenceNode>("CSG", "Difference", menu, &submenus);
    addNodeToMenu<ScriptNode>("Meta", "Script", menu, &submenus);
    addNodeToMenu<AttractNode>("Deform", "Attract", menu, &submenus);
    addNodeToMenu<RepelNode>("Deform", "Repel", menu, &submenus);
    addNodeToMenu<ScaleXNode>("Deform", "Scale (X)", menu, &submenus);
    addNodeToMenu<ScaleYNode>("Deform", "Scale (Y)", menu, &submenus);
    addNodeToMenu<ScaleZNode>("Deform", "Scale (Z)", menu, &submenus);
    addNodeToMenu<RotateXNode>("Transform", "Rotate (X)", menu, &submenus);
    addNodeToMenu<RotateYNode>("Transform", "Rotate (Y)", menu, &submenus);
    addNodeToMenu<RotateZNode>("Transform", "Rotate (Z)", menu, &submenus);
    addNodeToMenu<RecenterNode>("Transform", "Recenter", menu, &submenus);
    addNodeToMenu<TranslateNode>("Transform", "Translate", menu, &submenus);
    addNodeToMenu<SliderNode>("Variable", "Slider", menu, &submenus);
    addNodeToMenu<Iterate2DNode>("Iterate", "Iterate (2D)", menu, &submenus);
    // NODE CASES
}
