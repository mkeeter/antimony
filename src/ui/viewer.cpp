#include <Python.h>

#include <QDebug>

#include <QPropertyAnimation>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QTimer>

#include "ui_viewer.h"

#include "ui/canvas.h"
#include "ui/viewer.h"
#include "datum/datum.h"
#include "control/control.h"
#include "node/node.h"

////////////////////////////////////////////////////////////////////////////////

NodeViewer::NodeViewer(Control* control)
    : QWidget(NULL), ui(new Ui::NodeViewer)
{
    ui->setupUi(this);
    ui->title->setText(QString("<b>") +
                       control->getNode()->metaObject()->className() +
                       QString("</b>"));
    populateGrid(control->getNode());

    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(deleteLater()));

    proxy = control->scene()->addWidget(this);
    proxy->setZValue(-2);
    proxy->setFocusPolicy(Qt::TabFocus);
}

NodeViewer::~NodeViewer()
{
    delete ui;
}

void NodeViewer::paintEvent(QPaintEvent *)
{
    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
    proxy->update();
}

void NodeViewer::populateGrid(Node *node)
{
    for (Datum* d : node->findChildren<Datum*>())
    {
        if (d->objectName().startsWith("_"))
            continue;
        int row = ui->grid->rowCount();
        ui->grid->addWidget(new QLabel(d->objectName()), row, 1, Qt::AlignRight);
        ui->grid->addWidget(new _DatumLineEdit(d), row, 2);
    }
}

////////////////////////////////////////////////////////////////////////////////

_DatumLineEdit::_DatumLineEdit(Datum *datum, QWidget *parent)
{
    // Nothing to do here
}

void _DatumLineEdit::onDatumChanged()
{
    // Do nothing here yet
}

