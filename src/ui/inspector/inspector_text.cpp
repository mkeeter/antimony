#include <Python.h>

#include <QTextDocument>
#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QToolTip>
#include <QKeyEvent>

#include "ui/inspector/inspector_text.h"
#include "ui/colors.h"

#include "datum/datum.h"
#include "datum/eval_datum.h"

DatumTextItem::DatumTextItem(Datum* datum, QGraphicsItem* parent)
    : QGraphicsTextItem(parent), d(datum), txt(document()),
      background(Colors::base02), border(background)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setTextWidth(150);
    connect(datum, SIGNAL(changed()), this, SLOT(onDatumChanged()));
    onDatumChanged();

    bbox = boundingRect();
    connect(txt, SIGNAL(contentsChanged()), this, SLOT(onTextChanged()));

    installEventFilter(this);
}

void DatumTextItem::onDatumChanged()
{
    if (d->canEdit())
    {
        setDefaultTextColor(Colors::base04);
    }
    else
    {
        setDefaultTextColor(Colors::base03);
    }

    QTextCursor cursor = textCursor();
    int p = textCursor().position();
    txt->setPlainText(d->getString());
    cursor.setPosition(p);
    setTextCursor(cursor);

    setEnabled(d->canEdit());

    if (d->getValid())
    {
        border = background;
    }
    else
    {
        border = Colors::red;
    }

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
    painter->drawRect(boundingRect());
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
