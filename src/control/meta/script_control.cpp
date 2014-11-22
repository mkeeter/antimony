#include <Python.h>

#include "control/meta/script_control.h"

#include "graph/node/node.h"
#include "graph/datum/datum.h"

#include "ui/colors.h"

ScriptControl::ScriptControl(Canvas *canvas, Node *node)
    : TextLabelControl("Script", Colors::violet, canvas, node)
{
    connect(node->getDatum("name"), SIGNAL(changed()),
            this, SLOT(redraw()));
    // Nothing to do here
}

QString ScriptControl::getLabel() const
{
    PyObject* name = node->getDatum("name")->getValue();
    wchar_t* w = PyUnicode_AsWideCharString(name, 0);
    Q_ASSERT(w);
    QString out = QString::fromWCharArray(w);
    PyMem_Free(w);
    return out;
}
