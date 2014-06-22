#include "view_selector.h"
#include "ui_view_selector.h"

ViewSelector::ViewSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewSelector)
{
    ui->setupUi(this);
}

ViewSelector::~ViewSelector()
{
    delete ui;
}
