#include <QKeySequence>

#include "ui_viewer.h"

#include "ui/viewer.h"

NodeViewer::NodeViewer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NodeViewer)
{
    ui->setupUi(this);
}

NodeViewer::~NodeViewer()
{
    delete ui;
}

