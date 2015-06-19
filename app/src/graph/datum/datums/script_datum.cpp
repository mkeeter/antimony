#include <Python.h>

#include <QRegExp>
#include <QStringList>

#include "graph/datum/datums/shape_datum.h"
#include "graph/datum/datums/shape_output_datum.h"
#include "graph/datum/datums/script_datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/int_datum.h"
#include "graph/datum/datums/string_datum.h"
#include "graph/datum/datums/float_output_datum.h"

#include "graph/node/node.h"
#include "graph/hooks/hooks.h"

#include "fab/fab.h"
#include "ui/util/colors.h"

ScriptDatum::ScriptDatum(QString name, Node* parent)
    : EvalDatum(name, parent), globals(NULL)
{
    // Nothing to do here
}

ScriptDatum::ScriptDatum(QString name, QString expr, Node* parent)
    : ScriptDatum(name, parent)
{
    setExpr(expr);
}

int ScriptDatum::getStartToken() const
{
    return Py_file_input;
}

void ScriptDatum::modifyGlobalsDict(PyObject* g)
{
    globals = g;
    old_ui = hooks::loadHooks(g, this);
    Colors::loadColors();
}

bool ScriptDatum::isValidName(QString name) const
{
    return name.size() && !name.startsWith("__") && !touched.contains(name);
}

void ScriptDatum::makeInput(QString name, PyTypeObject *type,
                            QString value)
{
    if (!isValidName(name))
        throw hooks::HookException("Invalid datum name");
    else if (type == fab::ShapeType && !value.isNull())
        throw hooks::HookException(
                "Cannot have default argument for Shape input.");

    Q_ASSERT(dynamic_cast<Node*>(parent()));
    Node* n = static_cast<Node*>(parent());
    Datum* d = n->getDatum(name);

    // Save that this datum is still present in the script
    touched.insert(name);

    const auto datum_type =
        (type == &PyFloat_Type)     ? DatumType::FLOAT :
        (type == &PyLong_Type)      ? DatumType::INT :
        (type == &PyUnicode_Type)   ? DatumType::STRING :
        (type == fab::ShapeType)    ? DatumType::SHAPE: -1;

    if (d != NULL && d->getDatumType() != datum_type)
    {
        delete d;
        d = NULL;
    }

    if (d == NULL)
    {
        datums_changed = true;

        Q_ASSERT(dynamic_cast<Node*>(parent()));
        auto n = static_cast<Node*>(parent());

        if (type == &PyFloat_Type)
            d = new FloatDatum(name, value.isNull() ? "0.0" : value, n);
        else if (type == &PyLong_Type)
            d = new IntDatum(name, value.isNull() ? "0" : value, n);
        else if (type == &PyUnicode_Type)
            d = new StringDatum(name, value.isNull() ? "hello" : value, n);
        else if (type == fab::ShapeType)
            d = new ShapeDatum(name, n);
        else
            throw hooks::HookException("Invalid datum type.");
    }

    // Unset then reset the parent to move this Datum to the
    // bottom of the list (this ensures that datums in Inspectors
    // appear in the order that they were written in the script).
    d->setParent(NULL);
    d->setParent(parent());

    // When this input changes, the script datum should update.
    if (connectUpstream(d) && d->getValid())
        PyDict_SetItemString(globals, name.toStdString().c_str(), d->getValue());
    else
        throw hooks::HookException("Accessed invalid datum value.");
}

void ScriptDatum::makeOutput(QString name, PyObject *out)
{
    if (!isValidName(name))
        throw hooks::HookException("Invalid datum name");

    Q_ASSERT(dynamic_cast<Node*>(parent()));
    Node* n = static_cast<Node*>(parent());
    Datum* d = n->getDatum(name);

    // Save that this datum is still present in the script
    touched.insert(name);

    const auto datum_type =
        (out->ob_type == &PyFloat_Type)  ? DatumType::FLOAT_OUTPUT :
        (out->ob_type == fab::ShapeType) ? DatumType::SHAPE_OUTPUT : -1;

    if (d != NULL && d->getDatumType() != datum_type)
    {
        delete d;
        d = NULL;
    }

    if (d == NULL)
    {
        datums_changed = true;

        Q_ASSERT(dynamic_cast<Node*>(parent()));
        auto n = static_cast<Node*>(parent());

        if (out->ob_type == fab::ShapeType)
            d = new ShapeOutputDatum(name, n);
        else if (out->ob_type == &PyFloat_Type)
            d = new FloatOutputDatum(name, n);
        else
            throw hooks::HookException("Invalid datum type");
    }

    // Unset then reset the parent to move this Datum to the
    // bottom of the list (this ensures that datums in Inspectors
    // appear in the order that they were written in the script).
    d->setParent(NULL);
    d->setParent(parent());

    static_cast<OutputDatum*>(d)->setNewValue(out);
}

void ScriptDatum::setTitle(QString title)
{
    static_cast<Node*>(parent())->setTitle(title);
}

PyObject* ScriptDatum::getCurrentValue()
{
    // Assert that there isn't any recursion going on here.
    Q_ASSERT(globals == NULL);

    // Reset all of the touched flags, both on datums and Controls
    touched.clear();
    emit(static_cast<Node*>(parent())->clearControlTouchedFlag());

    datums_changed = false;
    QStringList datums_before;
    for (auto o : parent()->findChildren<Datum*>(
                        QString(), Qt::FindDirectChildrenOnly))
        if (!o->objectName().startsWith("_"))
            datums_before.append(o->objectName());

    // Swap in a stringIO object for stdout, saving stdout in out
    PyObject* sys_mod = PyImport_ImportModule("sys");
    PyObject* io_mod = PyImport_ImportModule("io");
    PyObject* stdout_obj = PyObject_GetAttrString(sys_mod, "stdout");
    PyObject* stderr_obj = PyObject_GetAttrString(sys_mod, "stderr");
    PyObject* string_out = PyObject_CallMethod(io_mod, "StringIO", NULL);
    PyObject_SetAttrString(sys_mod, "stdout", string_out);
    PyObject_SetAttrString(sys_mod, "stderr", string_out);
    Q_ASSERT(!PyErr_Occurred());

    PyObject* out = EvalDatum::getCurrentValue();

    // Swap old_ui back in to make recursive calls work.
    if (old_ui)
    {
        PyObject* fab_mod = PyImport_ImportModule("fab");
        PyObject_SetAttrString(fab_mod, "ui", old_ui);
        Py_DECREF(fab_mod);
    }

    // Get the output from the StringIO object
    PyObject* s = PyObject_CallMethod(string_out, "getvalue", NULL);
    wchar_t* w = PyUnicode_AsWideCharString(s, NULL);
    Q_ASSERT(w);
    output = QString::fromWCharArray(w);
    PyMem_Free(w);

    // Swap stdout back into sys.stdout
    PyObject_SetAttrString(sys_mod, "stdout", stdout_obj);
    PyObject_SetAttrString(sys_mod, "stderr", stderr_obj);
    for (auto o : {sys_mod, io_mod, stdout_obj, stderr_obj, string_out, s})
        Py_DECREF(o);

    // Look at all of the datums (other than the script datum and other
    // reserved datums), deleting them if they have not been touched.
    bool datum_order_changed = false;
    if (out)
    {
        // Delete any datums that were not touched in the last update cycle,
        // and set datum_order_changed if the order has changed.
        for (auto d : parent()->findChildren<Datum*>(
                            QString(), Qt::FindDirectChildrenOnly))

        {
            QString name = d->objectName();
            if (d != this && name.size() && name.at(0) != '_' &&
                !touched.contains(name))
            {
                datums_changed = true;
                delete d;
            }

            datum_order_changed |= !name.startsWith("_") &&
                                   !datums_before.isEmpty() &&
                                    datums_before.takeFirst() != name;
        }
    }

    // Request that all untouched Controls delete themselves.
    emit(static_cast<Node*>(parent())->deleteUntouchedControls());

    globals = NULL;

    if (datums_changed)
        emit(static_cast<Node*>(parent())->datumsChanged());
    else if (datum_order_changed)
        emit(static_cast<Node*>(parent())->datumOrderChanged());

    // Filter out default arguments to input datums, to make the script
    // simpler to read (because input('x', float, 12.0f) looks odd when
    // x doesn't have a value of 12 anymore).
    QRegExp input("(.*input\\([^(),]+,[^(),]+),[^(),]+(\\).*)");
    while (input.exactMatch(expr))
    {
        auto out = input.capturedTexts();
        expr = out[1] + out[2];
    }
    return out;
}

