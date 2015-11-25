#include <Python.h>

#include <QTextDocument>
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "canvas/inspector/datum_editor.h"
#include "canvas/inspector/frame.h"
#include "canvas/inspector/row.h"

#include "app/colors.h"
#include "app/app.h"

#include "graph/datum.h"

////////////////////////////////////////////////////////////////////////////////

InspectorDatumEditor::InspectorDatumEditor(Datum* d, InspectorRow* parent)
    : QGraphicsTextItem(parent), datum(d), txt(document()),
      valid(true), recursing(false)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);

    // Propagate text document changes into the datum
    connect(txt, &QTextDocument::contentsChanged,
            [=]{ d->setText(txt->toPlainText().toStdString()); });

    installEventFilter(this);
}

////////////////////////////////////////////////////////////////////////////////

QString InspectorDatumEditor::trimFloat(QString t)
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

QString InspectorDatumEditor::formatSpecial(QString t, const DatumState& state)
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

void InspectorDatumEditor::update(const DatumState& state)
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
    setEnabled(state.editable);
    setDefaultTextColor(state.editable ? Colors::base04 : Colors::base03);

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

void InspectorDatumEditor::tweakValue(int dx)
{
    if (datum->isValid())
    {
        if (datum->getType() == &PyFloat_Type)
        {
            const double scale = fmax(
                    0.01, fabs(PyFloat_AsDouble(datum->currentValue()) * 0.01));
            dragFloat(scale * dx);
        }
        else if (datum->getType() == &PyLong_Type)
        {
            dragInt(dx);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

bool InspectorDatumEditor::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == this && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Tab || keyEvent->key() == Qt::Key_Return)
            emit(tabPressed(this));
        else if (keyEvent->key() == Qt::Key_Backtab)
            emit(shiftTabPressed(this));
        else if (keyEvent->matches(QKeySequence::Undo))
            App::instance()->undo();
        else if (keyEvent->matches(QKeySequence::Redo))
            App::instance()->redo();
        else if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down)
            tweakValue(keyEvent->key() == Qt::Key_Up ? 1 : -1);
        else
            return false;
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////

void InspectorDatumEditor::paint(QPainter* painter,
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

void InspectorDatumEditor::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    drag_start = QString::fromStdString(datum->getText());
    drag_accumulated = 0;

    QGraphicsTextItem::mousePressEvent(event);
}

void InspectorDatumEditor::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QString drag_end = QString::fromStdString(datum->getText());
    if (drag_start != drag_end)
    {
        /* XXX 
        App::instance()->pushStack(
                new UndoChangeExprCommand(d, drag_start, drag_end));
                */
    }

    QGraphicsTextItem::mouseReleaseEvent(event);
}

void InspectorDatumEditor::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (datum->isValid() && (event->modifiers() & Qt::ShiftModifier))
    {
        if (datum->getType() == &PyFloat_Type)
        {
            const double scale = fmax(
                    0.01, fabs(PyFloat_AsDouble(datum->currentValue()) * 0.01));
            const double dx = (event->screenPos() - event->lastScreenPos()).x();
            dragFloat(scale * dx);
            return;
        }
        else if (datum->getType() == &PyLong_Type)
        {
            drag_accumulated += (event->screenPos() -
                                 event->lastScreenPos()).x() / 30.;
            int q = drag_accumulated;
            drag_accumulated -= q;
            dragInt(q);
            return;
        }
    }
    QGraphicsTextItem::mouseMoveEvent(event);
}

////////////////////////////////////////////////////////////////////////////////

void InspectorDatumEditor::dragFloat(float a)
{
    bool ok = false;

    QString s = QString::fromStdString(datum->getText());
    double v = s.toFloat(&ok);
    if (ok)
        datum->setText(QString::number(v + a).toStdString());
}

void InspectorDatumEditor::dragInt(int a)
{
    bool ok = false;

    QString s = QString::fromStdString(datum->getText());
    double i = s.toInt(&ok);
    if (ok)
        datum->setText(QString::number(i + a).toStdString());
}
