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
    makeAddMenu();
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

////////////////////////////////////////////////////////////////////////////////

#include "node/node.h"
#include "control/control.h"

template <class N, class C>
void MainWindow::createNew()
{
    QPoint mouse_pos  = canvas->rect().center();
    QPointF scene_pos = canvas->sceneRect().center();
    QVector3D obj_pos = canvas->sceneToWorld(scene_pos);

    QCursor::setPos(canvas->mapToGlobal(mouse_pos));

    Node* n = new N(obj_pos.x(), obj_pos.y(), obj_pos.z(),
                    100 / canvas->getScale());
    Control* c = new C(canvas, n);
    c->grabMouse();
    c->setClickPos(scene_pos);
}

template <class N, class C>
void MainWindow::addNodeToAddMenu(QString category, QString name,
                                  QMap<QString, QMenu*>* submenus)
{
    if (!submenus->contains(category))
    {
        (*submenus)[category] = ui->menuAdd->addMenu(category);
    }
    QAction* a = (*submenus)[category]->addAction(name);
    connect(a, &QAction::triggered, this, &MainWindow::createNew<N, C>);
}

#include "node/3d/point3d_node.h"
#include "node/3d/cube_node.h"
#include "node/2d/circle_node.h"

#include "control/3d/point3d_control.h"
#include "control/3d/cube_control.h"
#include "control/2d/circle_control.h"

void MainWindow::makeAddMenu()
{
    QMap<QString, QMenu*> submenus;
    addNodeToAddMenu<Point3D, Point3DControl>("3D", "Point", &submenus);
    addNodeToAddMenu<CubeNode, CubeControl>("3D", "Cube", &submenus);
    addNodeToAddMenu<CircleNode, CircleControl>("2D", "Circle", &submenus);
}
