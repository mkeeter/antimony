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

template <class N>
void MainWindow::createNew()
{
    QPoint mouse_pos  = canvas->rect().center();
    QPointF scene_pos = canvas->sceneRect().center();
    QVector3D obj_pos = canvas->sceneToWorld(scene_pos);

    QCursor::setPos(canvas->mapToGlobal(mouse_pos));

    Node* n = new N(obj_pos.x(), obj_pos.y(), obj_pos.z(),
                    100 / canvas->getScale());
    Control* c = NodeManager::manager()->makeControlFor(canvas, n);
    c->grabMouse();
    c->setClickPos(scene_pos);
}

template <class N>
void MainWindow::addNodeToMenu(QString category, QString name,
                                  QMenu* menu, QMap<QString, QMenu*>* submenus)
{
    if (!submenus->contains(category))
    {
        (*submenus)[category] = menu->addMenu(category);
    }
    QAction* a = (*submenus)[category]->addAction(name);
    connect(a, &QAction::triggered, this, &MainWindow::createNew<N>);
}

#include "node/3d/point3d_node.h"
#include "node/3d/cube_node.h"
#include "node/2d/circle_node.h"
#include "node/2d/point2d_node.h"
#include "node/2d/triangle_node.h"
#include "node/2d/text_node.h"
#include "node/meta/script_node.h"

void MainWindow::populateMenu(QMenu* menu)
{
    QMap<QString, QMenu*> submenus;
    addNodeToMenu<Point3D>("3D", "Point", menu, &submenus);
    addNodeToMenu<CubeNode>("3D", "Cube", menu, &submenus);
    addNodeToMenu<CircleNode>("2D", "Circle", menu, &submenus);
    addNodeToMenu<Point2D>("2D", "Point", menu, &submenus);
    addNodeToMenu<TriangleNode>("2D", "Triangle", menu, &submenus);
    addNodeToMenu<TextNode>("2D", "Text", menu, &submenus);
    addNodeToMenu<ScriptNode>("Meta", "Script", menu, &submenus);
}
