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
#include "datum/eval.h"

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
    : QLineEdit(parent), d(datum)
{
    onDatumChanged();
    connect(datum, SIGNAL(changed()), this, SLOT(onDatumChanged()));

    connect(this, SIGNAL(textEdited(QString)),
            this, SLOT(onTextChanged(QString)));
    // Nothing to do here
}

void _DatumLineEdit::onDatumChanged()
{
    int p = cursorPosition();
    setText(d->getString());
    setCursorPosition(p);

    setEnabled(d->canEdit());

    if (d->getValid())
    {
        setStyleSheet("QLineEdit:disabled { color: #ccc; }");
    }
    else
    {
        setStyleSheet("QLineEdit:disabled { color: #ccc; }\n"
                      "QLineEdit { background-color: #faa; }");
    }
}

void _DatumLineEdit::onTextChanged(QString txt)
{
    EvalDatum* e = dynamic_cast<EvalDatum*>(d);
    if (e && e->canEdit())
    {
        e->setExpr(txt);
    }
}
