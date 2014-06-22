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
