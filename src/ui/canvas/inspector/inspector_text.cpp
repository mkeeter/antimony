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
#include "graph/datum/datums/int_datum.h"
#include "graph/datum/types/eval_datum.h"

#include "app/app.h"
#include "app/undo/undo_change_expr.h"

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

    connect(document(), &QTextDocument::undoCommandAdded,
            this, &DatumTextItem::onUndoCommandAdded);

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
    if (!d->hasInput() && !d->canEdit())
        txt->setPlainText(d->getString() + " (output)");
    else
        txt->setPlainText(d->getString());

    if (p < d->getString().length())
    {
        cursor.setPosition(p);
        setTextCursor(cursor);
    }

    setEnabled(d->canEdit());

    if (d->getValid())
        border = background;
    else
        border = Colors::red;

    // Set tooltip if there was a Python evaluation error.
    if (dynamic_cast<EvalDatum*>(d) && !d->getValid())
    {
        setToolTip(static_cast<EvalDatum*>(d)->getErrorTraceback());
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

void DatumTextItem::onUndoCommandAdded()
{
    EvalDatum* e = dynamic_cast<EvalDatum*>(d);
    if (e && e->canEdit())
    {
        disconnect(document(), &QTextDocument::contentsChanged,
                   this, &DatumTextItem::onTextChanged);

        document()->undo();
        QString before = document()->toPlainText();
        int cursor_before = textCursor().position();

        document()->redo();
        QString after = document()->toPlainText();
        int cursor_after = textCursor().position();

        App::instance()->pushStack(
                new UndoChangeExprCommand(
                    e, before, after,
                    cursor_before, cursor_after, this));

        connect(document(), &QTextDocument::contentsChanged,
                this, &DatumTextItem::onTextChanged);
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
    if (obj == this && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Return)
            emit tabPressed(this);
        else if (keyEvent->key() == Qt::Key_Backtab)
            emit shiftTabPressed(this);
        else if (keyEvent->matches(QKeySequence::Undo))
            App::instance()->undo();
        else if (keyEvent->matches(QKeySequence::Redo))
            App::instance()->redo();
        else if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down) {
            const int dx = keyEvent->key() == Qt::Key_Up ? 1 : -1;
            const auto f = dynamic_cast<FloatDatum*>(d);
            const auto i = dynamic_cast<IntDatum*>(d);
            if (f && f->getValid())
            {
                const double scale = fmax(
                        0.01, abs(PyFloat_AsDouble(f->getValue()) * 0.01));
                f->dragValue(scale * dx);
            }
            else if (i && i->getValid())
            {
                i->dragValue(dx);
            }
        }
        else
            return false;
        return true;
    }
    return false;
}

void DatumTextItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (const auto e = dynamic_cast<EvalDatum*>(d))
    {
        drag_start = e->getExpr();
        drag_accumulated = 0;
    }

    QGraphicsTextItem::mousePressEvent(event);
}

void DatumTextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (const auto e = dynamic_cast<EvalDatum*>(d))
    {
        QString drag_end = e->getExpr();
        if (drag_start != drag_end)
            App::instance()->pushStack(
                    new UndoChangeExprCommand(e, drag_start, drag_end));
    }

    QGraphicsTextItem::mouseReleaseEvent(event);
}

void DatumTextItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    const auto f = dynamic_cast<FloatDatum*>(d);
    const auto i = dynamic_cast<IntDatum*>(d);
    if (f && f->getValid() && (event->modifiers() & Qt::ShiftModifier))
    {
        const double scale = fmax(
                0.01, abs(PyFloat_AsDouble(f->getValue()) * 0.01));
        const double dx = (event->screenPos() - event->lastScreenPos()).x();
        f->dragValue(scale * dx);
    }
    else if (i && i->getValid() && (event->modifiers() & Qt::ShiftModifier))
    {
        drag_accumulated += (event->screenPos() -
                             event->lastScreenPos()).x() / 30.;
        int q = drag_accumulated;
        drag_accumulated -= q;
        i->dragValue(q);
    }
    else
    {
        QGraphicsTextItem::mouseMoveEvent(event);
    }
}
