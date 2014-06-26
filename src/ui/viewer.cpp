#include <Python.h>

#include <QDebug>

#include <QPropertyAnimation>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>

#include "ui_viewer.h"

#include "ui/canvas.h"
#include "ui/viewer.h"
#include "datum/datum.h"
#include "control/control.h"
#include "node/node.h"

NodeViewer::NodeViewer(Control* control)
    : QWidget(NULL), ui(new Ui::NodeViewer), _mask_size(0)
{
    ui->setupUi(this);
    ui->title->setText(QString("<b>") +
                       control->getNode()->metaObject()->className() +
                       QString("</b>"));
    populateGrid(control->getNode());

    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(animateClose()));

    // Fix weird graphics glitches related to proxy widget
    connect(ui->closeButton, SIGNAL(pressed()), this, SLOT(redrawProxy()));
    connect(ui->closeButton, SIGNAL(released()), this, SLOT(redrawProxy()));

    proxy = control->scene()->addWidget(this);
    proxy->setZValue(-2);
    proxy->setFocusPolicy(Qt::TabFocus);
    animateOpen();
}

NodeViewer::~NodeViewer()
{
    delete ui;
}

float NodeViewer::getMaskSize() const
{
    return _mask_size;
}

void NodeViewer::setMaskSize(float m)
{
    _mask_size = m;
    QSize full = sizeHint();
    setMask(QRegion(0, 0, full.width()*m + 1, full.height()*m + 1));
    proxy->update();

    qDebug() << m;
}

void NodeViewer::redrawProxy()
{
    proxy->update();
}

void NodeViewer::animateClose()
{
    // Take focus away from text entry box to prevent graphical artifacts
    setFocus();
    QPropertyAnimation* a = new QPropertyAnimation(this, "mask_size", this);
    a->setDuration(100);
    a->setStartValue(1);
    a->setEndValue(0);
    connect(a, SIGNAL(finished()), this, SLOT(deleteLater()));
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void NodeViewer::animateOpen()
{
    QPropertyAnimation* a = new QPropertyAnimation(this, "mask_size", this);
    a->setDuration(100);
    a->setStartValue(0);
    a->setEndValue(1);
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void NodeViewer::paintEvent(QPaintEvent *)
{
    QStyleOption o;
    o.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &o, &p, this);
}

void NodeViewer::populateGrid(Node *node)
{
    // Nothing to do here.. yet
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
