#include <Python.h>

#include <QKeySequence>

#include "ui_main_window.h"

#include "ui/main_window.h"
#include "ui/canvas.h"
#include "ui/script/script_editor.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    canvas = ui->canvas;
    populateMenu(ui->menuAdd);

    setWindowTitle("antimony");
}

MainWindow::~MainWindow()
{
    delete ui;
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
    new ScriptEditorItem(d, ui->canvas);
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

template <Node* (*f)(float, float, float, float, QObject*), bool recenter>
void MainWindow::createNew()
{
    QPoint mouse_pos = recenter
        ? canvas->rect().center()
        : canvas->mapFromGlobal(QCursor::pos());
    QPointF scene_pos = canvas->mapToScene(mouse_pos);
    QVector3D obj_pos = canvas->sceneToWorld(scene_pos);

    if (recenter)
    {
        QCursor::setPos(canvas->mapToGlobal(mouse_pos));
    }

    Node* n = f(obj_pos.x(), obj_pos.y(), obj_pos.z(),
                100 / canvas->getScale(), NULL);
    Control* c = Control::makeControlFor(canvas, n);
    c->grabMouse();
    c->setClickPos(scene_pos);
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
    {
        _populateMenu<true>(menu);
    }
    else
    {
        _populateMenu<false>(menu);
    }

}
