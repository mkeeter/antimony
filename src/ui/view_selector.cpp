#include <cmath>

#include "ui/canvas.h"
#include "ui/main_window.h"
#include "ui/view_selector.h"

#include "ui_view_selector.h"

ViewSelector::ViewSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewSelector)
{
    ui->setupUi(this);
    connect(ui->top, SIGNAL(pressed()), this, SLOT(onTopPressed()));
    connect(ui->bottom, SIGNAL(pressed()), this, SLOT(onBottomPressed()));
    connect(ui->right, SIGNAL(pressed()), this, SLOT(onRightPressed()));
    connect(ui->left, SIGNAL(pressed()), this, SLOT(onLeftPressed()));
    connect(ui->front, SIGNAL(pressed()), this, SLOT(onFrontPressed()));
    connect(ui->back, SIGNAL(pressed()), this, SLOT(onBackPressed()));
}

ViewSelector::~ViewSelector()
{
    delete ui;
}

void ViewSelector::spinTo(float pitch, float yaw)
{
    dynamic_cast<MainWindow*>(parent()->parent())->getCanvas()->spinTo(
            pitch, yaw);
}

void ViewSelector::onTopPressed()
{
    spinTo(0, 0);
}

void ViewSelector::onBottomPressed()
{
    spinTo(0, -M_PI);
}

void ViewSelector::onLeftPressed()
{
    spinTo(M_PI/2, -M_PI/2);
}

void ViewSelector::onRightPressed()
{
    spinTo(-M_PI/2, -M_PI/2);
}

void ViewSelector::onBackPressed()
{
    spinTo(-M_PI, -M_PI/2);
}

void ViewSelector::onFrontPressed()
{
    spinTo(0, -M_PI/2);
}
