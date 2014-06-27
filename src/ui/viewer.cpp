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
    : QWidget(NULL), ui(new Ui::NodeViewer), _mask_size(0)
{
    ui->setupUi(this);
    ui->title->setText(QString("<b>") +
                       control->getNode()->metaObject()->className() +
                       QString("</b>"));
    populateGrid(control->getNode());

    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(animateClose()));

    proxy = control->scene()->addWidget(this);
    proxy->setZValue(-2);
    proxy->setFocusPolicy(Qt::TabFocus);
    animateOpen();

    installEventFilter(new _DrawFilter(this));
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
}

void NodeViewer::redraw()
{
    update();
    //proxy->update();
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

////////////////////////////////////////////////////////////////////////////////

_DrawFilter::_DrawFilter(NodeViewer *viewer)
    : QObject(viewer), viewer(viewer)
{
    // Nothing to do here.
}

bool _DrawFilter::eventFilter(QObject *o, QEvent *e)
{
    Q_UNUSED(o);
    if (e->type() == QEvent::Paint)
    {
        QTimer::singleShot(0, viewer, SLOT(redraw()));
    }
    return false;
}
