#include <Python.h>

#include <QDebug>

#include <QPropertyAnimation>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QTimer>
#include <QTextDocument>

#include "ui/canvas.h"
#include "ui/viewer.h"

#include "datum/datum.h"
#include "datum/eval.h"

#include "control/control.h"
#include "node/node.h"

//////////////////////////////////////////r/////////////////////////////////////

NodeViewer::NodeViewer(Control* control)
{
    populateLists(control->getNode());
    control->scene()->addItem(this);
    setZValue(-2);
    setFlag(ItemClipsChildrenToShape);
}

float NodeViewer::labelWidth() const
{
    float label_width = 0;
    for (auto label : labels)
    {
        label_width = fmax(label_width, label->boundingRect().width());
    }
    return label_width;
}

QRectF NodeViewer::boundingRect() const
{
    float height = 0;
    float width = labelWidth() + 10 + 150 + 5;

    for (auto e : editors)
    {
        height += e->boundingRect().height() + 6;
    }
    return QRectF(0, 0, width, height);
}

void NodeViewer::onLayoutChanged()
{
    float label_width = labelWidth();
    float x = 0;
    float y = 0;
    for (int i=0; i < labels.length(); ++i)
    {
        float dy = fmax(labels[i]->boundingRect().height(),
                        editors[i]->boundingRect().height());
        labels[i]->setPos(x + label_width - labels[i]->boundingRect().width(),
                          y + 3);
        editors[i]->setPos(x + label_width + 10, y + 3);
        y += dy + 6;
    }
}

void NodeViewer::populateLists(Node *node)
{
    for (Datum* d : node->findChildren<Datum*>())
    {
        if (d->objectName().startsWith("_"))
            continue;
        labels << new QGraphicsTextItem(d->objectName(), this);
        editors << new _DatumTextItem(d, this);
        connect(editors.back(), SIGNAL(boundsChanged()),
                this, SLOT(onLayoutChanged()));
    }
    onLayoutChanged();
}

void NodeViewer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(QColor("#ddd"));
    painter->setPen(Qt::NoPen);
    painter->drawRect(boundingRect());
}

////////////////////////////////////////////////////////////////////////////////

_DatumTextItem::_DatumTextItem(Datum* datum, QGraphicsItem* parent)
    : QGraphicsTextItem("Helloooo", parent), d(datum), txt(document()),
      background(Qt::white)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setTextWidth(150);
    connect(datum, SIGNAL(changed()), this, SLOT(onDatumChanged()));
    onDatumChanged();

    bbox = boundingRect();
    connect(txt, SIGNAL(contentsChanged()), this, SLOT(onTextChanged()));
}

void _DatumTextItem::onDatumChanged()
{
    QTextCursor cursor = textCursor();
    int p = textCursor().position();
    txt->setPlainText(d->getString());
    cursor.setPosition(p);
    setTextCursor(cursor);

    setEnabled(d->canEdit());

    if (d->getValid())
    {
        background = Qt::white;
    }
    else
    {
        background = QColor("#faa");
    }

}

void _DatumTextItem::onTextChanged()
{
    if (bbox != boundingRect())
    {
        bbox = boundingRect();
        emit boundsChanged();
    }

    EvalDatum* e = dynamic_cast<EvalDatum*>(d);
    if (e && e->canEdit())
    {
        e->setExpr(txt->toPlainText());
    }
}

void _DatumTextItem::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* o,
                           QWidget* w)
{
    painter->setBrush(background);
    painter->setPen(Qt::NoPen);
    painter->drawRect(boundingRect());
    QGraphicsTextItem::paint(painter, o, w);
};
