#pragma once

#include <Python.h>

#include <QPlainTextEdit>
#include <QPointer>

#include "undo/undo_catcher.h"
#include "undo/undo_change_script.h"

struct Script;
class ScriptNode;

class ScriptEditor:
    public UndoCatcher<QPlainTextEdit, QWidget, ScriptNode, UndoChangeScript>
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
};
