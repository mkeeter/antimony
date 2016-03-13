#pragma once

#include <QTextDocument>
#include <QKeyEvent>

#include "app/app.h"

/*
 *  This class wraps a text editor T (with parent P),
 *  some kind of undo target G, and an undo command U.
 *
 *  It automatically detects changes in the text field
 *  and pushes undo commands to the global undo stack.
 *
 *  In addition, it overloads undo / redo keyboard shortcuts,
 *  routing them through the global undo / redo system.
 */
template <class T, class P, class G, class U>
class UndoCatcher : public T
{
public:
    UndoCatcher(G* target, P* parent=NULL)
        : T(parent), target(target)
    {
        T::connect(T::document(), &QTextDocument::undoCommandAdded,
                   this, &UndoCatcher<T, P, G, U>::onUndoCommandAdded);
        T::installEventFilter(this);
    }

protected slots:
    void onUndoCommandAdded()
    {
        T::document()->blockSignals(true);

        auto doc = T::document();
        doc->undo();
        const QString before = doc->toPlainText();
        const int cursor_before = T::textCursor().position();

        doc->redo();
        const QString after = doc->toPlainText();
        const int cursor_after = T::textCursor().position();

        App::instance()->pushUndoStack(
                new U(target, before, after,
                      this, cursor_before, cursor_after));
        T::document()->blockSignals(false);
    }

protected:
    bool eventFilter(QObject* obj, QEvent* event)
    {
        if (obj == this && event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->matches(QKeySequence::Undo))
            {
                App::instance()->undo();
                return true;
            }
            else if (keyEvent->matches(QKeySequence::Redo))
            {
                App::instance()->redo();
                return true;
            }
        }
        return false;
    }

    G* const target;
};
