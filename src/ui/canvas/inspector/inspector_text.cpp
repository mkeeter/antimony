#include <Python.h>

#include <QTextDocument>
#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QToolTip>
#include <QKeyEvent>

#include "ui/canvas/inspector/inspector_text.h"
#include "ui/util/colors.h"

#include "graph/datum/datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/types/eval_datum.h"

DatumTextItem::DatumTextItem(Datum* datum, QGraphicsItem* parent)
    : QGraphicsTextItem(parent), d(datum), txt(document()),
      background(Colors::base02), foreground(Colors::base04),
      border(background)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setTextWidth(150);
    connect(datum, &Datum::changed, this, &DatumTextItem::onDatumChanged);
    onDatumChanged();

    bbox = boundingRect();
    connect(txt, &QTextDocument::contentsChanged,
            this, &DatumTextItem::onTextChanged);

    installEventFilter(this);
}

void DatumTextItem::setAsTitle()
{
    background = Colors::base03;
    foreground = Colors::base06;

    auto f = font();
    f.setBold(true);
    setFont(f);

    // Allow this item to grow horizontally forever
    setTextWidth(-1);

    // Force a redraw
    onDatumChanged();
}

void DatumTextItem::onDatumChanged()
{
    if (d->canEdit())
        setDefaultTextColor(foreground);
    else
        setDefaultTextColor(Colors::base03);

    QTextCursor cursor = textCursor();
    int p = textCursor().position();
    txt->setPlainText(d->getString());
    cursor.setPosition(p);
    setTextCursor(cursor);

    setEnabled(d->canEdit());

    if (d->getValid())
        border = background;
    else
        border = Colors::red;

    // Set tooltip if there was a Python evaluation error.
    if (dynamic_cast<EvalDatum*>(d) && !d->getValid())
    {
        setToolTip(dynamic_cast<EvalDatum*>(d)->getErrorTraceback());
    }
    else
    {
        setToolTip("");
        QToolTip::hideText();
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
    painter->setPen(QPen(border, 2));
    painter->drawRect(boundingRect().adjusted(-1, 1, 1, -1));
    QGraphicsTextItem::paint(painter, o, w);
}

bool DatumTextItem::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == this)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Tab)
            {
                emit tabPressed(this);
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Backtab)
            {
                emit shiftTabPressed(this);
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Return)
            {
                emit returnPressed();
                return true;
            }
        }
        return false;
    }
    return DatumTextItem::eventFilter(obj, event);
}

void DatumTextItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    const auto f = dynamic_cast<FloatDatum*>(d);
    if (f && f->getValid() && (event->modifiers() & Qt::ShiftModifier))
    {
        const double scale = fmax(
                0.01, abs(PyFloat_AsDouble(f->getValue()) * 0.01));
        const double dx = (event->screenPos() - event->lastScreenPos()).x();
        f->dragValue(scale * dx);
    }
    else
    {
        QGraphicsTextItem::mouseMoveEvent(event);
    }
}
