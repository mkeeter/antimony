#include <Python.h>

#include <QTextDocument>
#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include "ui/inspector/inspector_text.h"
#include "datum/datum.h"
#include "datum/eval.h"

DatumTextItem::DatumTextItem(Datum* datum, QGraphicsItem* parent)
    : QGraphicsTextItem(parent), d(datum), txt(document()),
      background(Qt::white)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setTextWidth(150);
    connect(datum, SIGNAL(changed()), this, SLOT(onDatumChanged()));
    onDatumChanged();

    bbox = boundingRect();
    connect(txt, SIGNAL(contentsChanged()), this, SLOT(onTextChanged()));
}

void DatumTextItem::onDatumChanged()
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

void DatumTextItem::onTextChanged()
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

void DatumTextItem::paint(QPainter* painter,
                           const QStyleOptionGraphicsItem* o,
                           QWidget* w)
{
    painter->setBrush(background);
    painter->setPen(Qt::NoPen);
    painter->drawRect(boundingRect());
    QGraphicsTextItem::paint(painter, o, w);
}
