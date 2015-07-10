#include <Python.h>

#include <QTextDocument>
#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QToolTip>
#include <QKeyEvent>

#include "ui/canvas/inspector/inspector_text.h"
#include "ui/util/colors.h"

#include "graph/datum.h"

#include "app/app.h"
#include "app/undo/undo_change_expr.h"

DatumTextItem::DatumTextItem(Datum* datum, QGraphicsItem* parent)
    : QGraphicsTextItem(parent), d(datum), txt(document()),
      background(Colors::base02), foreground(Colors::base04),
      border(background)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setTextWidth(150);

    d->installWatcher(this);

    bbox = boundingRect();
    connect(txt, &QTextDocument::contentsChanged,
            this, &DatumTextItem::onTextChanged);

    connect(document(), &QTextDocument::undoCommandAdded,
            this, &DatumTextItem::onUndoCommandAdded);

    installEventFilter(this);

    // Force a redraw
    trigger(d->getState());
}

void DatumTextItem::trigger(const NodeState& state)
{
    // Nothing to do here
}

void DatumTextItem::trigger(const DatumState& state)
{
    setDefaultTextColor(state.editable ? foreground : Colors::base03);

    QTextCursor cursor = textCursor();
    int p = textCursor().position();
    txt->setPlainText(QString::fromStdString(state.text));

    if (p < state.text.length())
    {
        cursor.setPosition(p);
        setTextCursor(cursor);
    }

    setEnabled(state.editable);

    border = state.valid ? background : Colors::red;

    // Set tooltip if there was a Python evaluation error.
    if (!state.valid)
    {
        setToolTip(QString::fromStdString(state.error));
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

    // If we're allowed to edit,
    d->setText(txt->toPlainText().toStdString());
}

void DatumTextItem::onUndoCommandAdded()
{
    /*
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
    */
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
            /*
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
            */
        }
        else
            return false;
        return true;
    }
    return false;
}

void DatumTextItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    /*
    if (const auto e = dynamic_cast<EvalDatum*>(d))
    {
        drag_start = e->getExpr();
        drag_accumulated = 0;
    }
    */

    QGraphicsTextItem::mousePressEvent(event);
}

void DatumTextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    /*
    if (const auto e = dynamic_cast<EvalDatum*>(d))
    {
        QString drag_end = e->getExpr();
        if (drag_start != drag_end)
            App::instance()->pushStack(
                    new UndoChangeExprCommand(e, drag_start, drag_end));
    }
    */

    QGraphicsTextItem::mouseReleaseEvent(event);
}

void DatumTextItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    /*
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
    */
    {
        QGraphicsTextItem::mouseMoveEvent(event);
    }
}
