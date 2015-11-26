#pragma once

#include <QPlainTextEdit>
#include <QPointer>

struct Script;
class ScriptNode;

class ScriptEditor: public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit ScriptEditor(Script* script, QWidget* parent=NULL);

    /*
     *  Set the script text (without triggering a callback)
     */
    void setText(QString text);

    /*
     *  Highlights the given line in red
     */
    void highlightError(int lineno);

protected slots:
    void onUndoCommandAdded();

protected:
    /*
     *  Filter out control+Z events.
     */
    bool eventFilter(QObject* obj, QEvent* event);

    /*
     *  Store the script node so that we can make undo / redo actions
     */
    ScriptNode* const node;
};
