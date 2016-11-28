#include <Python.h>

#include <QTextDocument>
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "canvas/datum_editor.h"
#include "canvas/inspector/frame.h"

#include "app/colors.h"
#include "app/app.h"

#include "graph/datum.h"

////////////////////////////////////////////////////////////////////////////////

DatumEditor::DatumEditor(Datum* d, QGraphicsItem* parent)
    : UndoCatcher(d, parent), txt(document()),
      valid(true), recursing(false)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);

    // Propagate text document changes into the datum
    connect(txt, &QTextDocument::contentsChanged,
            [=]{ this->setDatumText(txt->toPlainText()); });

    setDefaultTextColor(Colors::base04);
    installEventFilter(this);
}

////////////////////////////////////////////////////////////////////////////////

QString DatumEditor::trimFloat(QString t)
{
    // Use QString to truncate floats to a sane number of decimal places
    QRegularExpression num("([0-9]+)\\.([0-9]+)");
    QRegularExpressionMatch match = num.match(t);
    if (match.isValid() && match.captured(2).size() > 6)
    {
        auto decimals = match.captured(2);
        decimals.truncate(6);
        return match.captured(1) + "." + decimals;
    }
    return t;
}

QString DatumEditor::formatSpecial(QString t, const DatumState& state) const
{
    // Special-case to avoid printing long shapes
    if (t.startsWith("fab.types.Shape"))
        t = "Shape";

    if (state.sigil == Datum::SIGIL_CONNECTION)
        t += state.links.size() > 1 ? " [links]" : " [link]";
    else if (state.sigil == Datum::SIGIL_OUTPUT)
        t += " [output]";

    return t;
}

void DatumEditor::update(const DatumState& state)
{

    {   // Update editor's text, holding cursor position if possible
        const QString t = (state.sigil == Datum::SIGIL_NONE)
                    ? trimFloat(QString::fromStdString(state.text))
                    : formatSpecial(QString::fromStdString(state.repr), state);

        QTextCursor cursor = textCursor();
        size_t p = cursor.position();

        txt->blockSignals(true);
        txt->setPlainText(t);
        txt->blockSignals(false);

        // Restore cursor position
        if (p < state.text.length())
        {
            cursor.setPosition(p);
            setTextCursor(cursor);
        }
    }

    // Set editable and adjust text color
    setEnabled(state.sigil == Datum::SIGIL_NONE);
    setDefaultTextColor(isEnabled() ? Colors::base04 : Colors::base03);

    // Store validity and set tooltip if there was a Python evaluation error.
    valid = state.valid;
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

////////////////////////////////////////////////////////////////////////////////

void DatumEditor::tweakValue(int dx)
{
    if (target->isValid())
    {
        if (target->getType() == &PyFloat_Type)
        {
            const double scale = fmax(
                    0.01, fabs(PyFloat_AsDouble(target->currentValue()) * 0.01));
            dragFloat(scale * dx);
        }
        else if (target->getType() == &PyLong_Type)
        {
            dragInt(dx);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

bool DatumEditor::eventFilter(QObject* obj, QEvent* event)
{
    if (!UndoCatcher::eventFilter(obj, event) &&
        obj == this && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Return)
        {
            emit(tabPressed(this));
            return true;
        }
        else if (keyEvent->key() == Qt::Key_Backtab)
        {
            emit(shiftTabPressed(this));
            return true;
        }
        else if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down)
        {
            tweakValue(keyEvent->key() == Qt::Key_Up ? 1 : -1);
            return true;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

void DatumEditor::paint(QPainter* painter,
                               const QStyleOptionGraphicsItem* o,
                               QWidget* w)
{
    painter->setBrush(Colors::base02);
    if (valid)
        painter->setPen(Qt::NoPen);
    else
        painter->setPen(QPen(Colors::red, 2));
    painter->drawRect(boundingRect().adjusted(-1, 1, 1, -1));
    QGraphicsTextItem::paint(painter, o, w);
}

////////////////////////////////////////////////////////////////////////////////

void DatumEditor::setDatumText(QString s)
{
    target->setText(s.toStdString());
}

QString DatumEditor::getDatumText() const
{
    return QString::fromStdString(target->getText());
}

////////////////////////////////////////////////////////////////////////////////

void DatumEditor::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // Use bare getText (instead of subgraph-safe getDatumText)
    // because we're creating an undo / redo event here
    drag_start = QString::fromStdString(target->getText());
    drag_accumulated = 0;

    QGraphicsTextItem::mousePressEvent(event);
}

void DatumEditor::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // Use bare getText (instead of subgraph-safe getDatumText)
    // because we're creating an undo / redo event here
    QString drag_end = QString::fromStdString(target->getText());
    if (drag_start != drag_end)
        App::instance()->pushUndoStack(
                new UndoChangeExpr(target, drag_start, drag_end));

    QGraphicsTextItem::mouseReleaseEvent(event);
}

void DatumEditor::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (target->isValid() && (event->modifiers() & Qt::ShiftModifier))
    {
        if (target->getType() == &PyFloat_Type)
        {
            const double s = (event->modifiers() & Qt::AltModifier) ? 0.03 : 0.01;
            const double scale = fmax(
                    s, fabs(PyFloat_AsDouble(target->currentValue()) * s));
            const double dx = (event->screenPos() - event->lastScreenPos()).x();
            dragFloat(scale * dx);
            return;
        }
        else if (target->getType() == &PyLong_Type)
        {
            const double s = (event->modifiers() & Qt::AltModifier) ? 10 : 30;
            drag_accumulated += (event->screenPos() -
                                 event->lastScreenPos()).x() / s;
            int q = drag_accumulated;
            drag_accumulated -= q;
            dragInt(q);
            return;
        }
    }
    QGraphicsTextItem::mouseMoveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////

void DatumEditor::dragFloat(float a)
{
    bool ok = false;

    QString s = getDatumText();
    double v = s.toFloat(&ok);
    if (ok)
        setDatumText(QString::number(v + a));
}

void DatumEditor::dragInt(int a)
{
    bool ok = false;

    QString s = getDatumText();
    double i = s.toInt(&ok);
    if (ok)
        setDatumText(QString::number(i + a));
}
