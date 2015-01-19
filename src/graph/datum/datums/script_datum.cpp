#include <Python.h>

#include <QRegExp>
#include <QStringList>

#include "graph/datum/wrapper.h"
#include "graph/datum/datums/shape_input_datum.h"
#include "graph/datum/datums/shape_output_datum.h"
#include "graph/datum/datums/script_datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/int_datum.h"
#include "graph/datum/datums/float_output_datum.h"

#include "graph/node/node.h"

#include "fab/fab.h"

ScriptDatum::ScriptDatum(QString name, Node* parent)
    : EvalDatum(name, parent), globals(NULL),
      input_func(scriptInput(this)), output_func(scriptOutput(this)),
      title_func(scriptTitle(this))
{
    // Nothing to do here
}

ScriptDatum::ScriptDatum(QString name, QString expr, Node* parent)
    : ScriptDatum(name, parent)
{
    setExpr(expr);
}

ScriptDatum::~ScriptDatum()
{
    Py_DECREF(input_func);
    Py_DECREF(output_func);
    Py_DECREF(title_func);
}

int ScriptDatum::getStartToken() const
{
    return Py_file_input;
}

void ScriptDatum::modifyGlobalsDict(PyObject* g)
{
    globals = g;
    PyDict_SetItemString(g, "input", input_func);
    PyDict_SetItemString(g, "output", output_func);
    PyDict_SetItemString(g, "title", title_func);
}

bool ScriptDatum::isValidName(QString name) const
{
    return name.size() && name.at(0) != '_' &&
           name != "script" && !touched.contains(name);
}

PyObject* ScriptDatum::makeInput(QString name, PyTypeObject *type,
                                 QString value)
{
    if (!isValidName(name))
    {
        PyErr_SetString(PyExc_RuntimeError, "Invalid datum name");
        return NULL;
    }
    else if (type == fab::ShapeType && !value.isNull())
    {
        PyErr_SetString(PyExc_RuntimeError,
                "Cannot have default argument for Shape input.");
        return NULL;
    }

    Node* n = dynamic_cast<Node*>(parent());
    Datum* d = n->getDatum(name);

    // Save that this datum is still present in the script
    touched.insert(name);

    const auto datum_type =
        (type == &PyFloat_Type)  ? DatumType::FLOAT :
        (type == &PyLong_Type)   ? DatumType::INT :
        (type == fab::ShapeType) ? DatumType::SHAPE_INPUT : 0;

    if (d != NULL && d->getDatumType() != datum_type)
    {
        delete d;
        d = NULL;
    }

    if (d == NULL)
    {
        datums_changed = true;

        auto n = dynamic_cast<Node*>(parent());
        Q_ASSERT(n);

        if (type == &PyFloat_Type)
        {
            d = new FloatDatum(name, value.isEmpty() ? "0.0" : value, n);
        }
        else if (type == &PyLong_Type)
        {
            d = new IntDatum(name, value.isEmpty() ? "0" : value, n);
        }
        else if (type == fab::ShapeType)
        {
            d = new ShapeInputDatum(name, n);
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "Invalid datum type");
            return NULL;
        }
    }
    d->setParent(NULL);
    d->setParent(parent());

    // When this input changes, the script datum should update.
    if (connectUpstream(d) && d->getValid())
    {
        PyDict_SetItemString(globals, name.toStdString().c_str(), d->getValue());
    }
    else
    {
        PyErr_SetString(PyExc_RuntimeError, "Accessed invalid datum value");
        return NULL;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* ScriptDatum::makeOutput(QString name, PyObject *out)
{
    if (!isValidName(name))
    {
        PyErr_SetString(PyExc_RuntimeError, "Invalid datum name");
        return NULL;
    }

    Node* n = dynamic_cast<Node*>(parent());
    Datum* d = n->getDatum(name);

    // Save that this datum is still present in the script
    touched.insert(name);

    const auto datum_type =
        (out->ob_type == &PyFloat_Type)  ? DatumType::FLOAT_OUTPUT :
        (out->ob_type == fab::ShapeType) ? DatumType::SHAPE_OUTPUT : 0;

    if (d != NULL && d->getDatumType() != datum_type)
    {
        delete d;
        d = NULL;
    }

    if (d == NULL)
    {
        datums_changed = true;

        auto n = dynamic_cast<Node*>(parent());
        Q_ASSERT(n);

        if (out->ob_type == fab::ShapeType)
        {
            d = new ShapeOutputDatum(name, n);
        }
        else if (out->ob_type == &PyFloat_Type)
        {
            d = new FloatOutputDatum(name, n);
        }
        else
        {
            PyErr_SetString(PyExc_TypeError, "Invalid datum type");
            return NULL;
        }
    }
    d->setParent(NULL);
    d->setParent(parent());

    static_cast<OutputDatum*>(d)->setNewValue(out);
    Py_INCREF(Py_None);
    return Py_None;

}

PyObject* ScriptDatum::setTitle(QString title)
{
    static_cast<Node*>(parent())->setTitle(title);
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* ScriptDatum::getCurrentValue()
{
    // Assert that there isn't any recursion going on here.
    Q_ASSERT(globals == NULL);

    touched.clear();
    datums_changed = false;

    // Swap in a stringIO object for stdout, saving stdout in out
    PyObject* sys_mod = PyImport_ImportModule("sys");
    PyObject* io_mod = PyImport_ImportModule("io");
    PyObject* stdout_obj = PyObject_GetAttrString(sys_mod, "stdout");
    PyObject* string_out = PyObject_CallMethod(io_mod, "StringIO", NULL);
    PyObject_SetAttrString(sys_mod, "stdout", string_out);
    Q_ASSERT(!PyErr_Occurred());

    PyObject* out = EvalDatum::getCurrentValue();

    // Get the output from the StringIO object
    PyObject* s = PyObject_CallMethod(string_out, "getvalue", NULL);
    wchar_t* w = PyUnicode_AsWideCharString(s, NULL);
    Q_ASSERT(w);
    output = QString::fromWCharArray(w);
    PyMem_Free(w);

    // Swap stdout back into sys.stdout
    PyObject_SetAttrString(sys_mod, "stdout", stdout_obj);
    Py_DECREF(sys_mod);
    Py_DECREF(io_mod);
    Py_DECREF(stdout_obj);
    Py_DECREF(string_out);
    Py_DECREF(s);

    // Look at all of the datums (other than the script datum and other
    // reserved datums), deleting them if they have not been touched.
    if (out)
    {
        for (auto d : parent()->findChildren<Datum*>())
        {
            QString name = d->objectName();
            if (d != this && name.size() && name.at(0) != '_' &&
                !touched.contains(name))
            {
                datums_changed = true;
                delete d;
            }
        }
    }

    globals = NULL;

    if (datums_changed)
        emit(static_cast<Node*>(parent())->datumsChanged());

    QRegExp input("(.*input\\([^(),]+,[^(),]+),[^(),]+(\\).*)");
    while (input.exactMatch(expr))
    {
        auto out = input.capturedTexts();
        expr = out[1] + out[2];
    }
    return out;
}

