#include <Python.h>

#include <QKeySequence>

#include "ui_main_window.h"

#include "ui/main_window.h"
#include "ui/canvas.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    addCanvas();
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

template <class T>
void MainWindow::createNew()
{
    QPoint mouse_pos  = canvas->rect().center();
    QPointF scene_pos = canvas->sceneRect().center();
    QVector3D obj_pos = canvas->sceneToWorld(scene_pos);

    QCursor::setPos(canvas->mapToGlobal(mouse_pos));

    Node* n = new T(obj_pos.x(), obj_pos.y(), obj_pos.z(),
                    100 / canvas->getScale());
    Control* control = n->makeControl(canvas);
    control->grabMouse();
    control->setClickPos(scene_pos);
}

template <class T>
void MainWindow::addNodeToAddMenu(QMap<QString, QMenu*> submenus)
{
    QString category = T::menuCategory();
    QString name = T::menuName();

    if (!submenus.contains(category))
    {
        submenus[category] = ui->menuAdd->addMenu(category);
        QAction* a = submenus[category]->addAction(name);
        connect(a, &QAction::triggered, this, &MainWindow::createNew<T>);
    }
}

#include "node/3d/point3d_node.h"
#include "node/2d/circle_node.h"

void MainWindow::makeAddMenu()
{
    QMap<QString, QMenu*> submenus;
    addNodeToAddMenu<Point3D>(submenus);
    addNodeToAddMenu<CircleNode>(submenus);
}
