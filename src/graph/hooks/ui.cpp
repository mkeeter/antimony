#include "graph/hooks/ui.h"
#include "graph/hooks/hooks.h"
#include "graph/node/node.h"
#include "graph/datum/datum.h"

#include "ui/viewport/viewport_scene.h"

#include "control/point.h"
#include "control/wireframe.h"

using namespace boost::python;

template <typename T, typename O>
QVector<T> ScriptUIHooks::_extractList(O obj)
{
    QVector<T> out;
    for (int i=0; i < len(obj); ++i)
    {
        extract<T> e(obj[i]);
        if (!e.check())
            throw hooks::HookException(
                    "Failed to extract data from object.");
        out << e();
    }
    return out;
}

template <typename T>
QVector<T> ScriptUIHooks::extractList(object obj)
{
    auto tuple_ = extract<tuple>(obj);
    if (tuple_.check())
        return _extractList<T>(tuple_());

    auto list_ = extract<list>(obj);
    if (list_.check())
        return _extractList<T>(list_());

    throw hooks::HookException("Input must be a list or a tuple");
}

QVector<QVector3D> ScriptUIHooks::extractVectors(object obj)
{
    QVector<QVector3D> out;

    // Try to extract a bunch of tuples from the top-level list.
    QVector<tuple> tuples;
    bool got_tuple = true;
    try {
        tuples = extractList<tuple>(obj);
    } catch (hooks::HookException e) {
        got_tuple = false;
    }

    if (got_tuple)
    {
        for (auto t : tuples)
        {
            auto v = extractList<float>(extract<object>(t)());
            if (v.length() != 3)
                throw hooks::HookException("Position data must have three terms.");
            out << QVector3D(v[0], v[1], v[2]);
        }
        return out;
    }

    // Try to extract a bunch of lists from the top-level list.
    QVector<list> lists;
    bool got_list = true;
    try {
        lists = extractList<list>(obj);
    } catch (hooks::HookException e) {
        got_list = false;
    }

    if (got_list)
    {
        for (auto l : lists)
        {
            auto v = extractList<float>(extract<object>(l)());
            if (v.length() != 3)
                throw hooks::HookException("Position data must have three terms.");
            out << QVector3D(v[0], v[1], v[2]);
        }
        return out;
    }

    throw hooks::HookException(
            "Position data must be a list of 3-element lists");
}

long ScriptUIHooks::getInstruction()
{
    // Get the current bytecode instruction
    // (used to uniquely identify calls to this function)
    auto inspect_module = PyImport_ImportModule("inspect");
    auto frame = PyObject_CallMethod(inspect_module, "currentframe", NULL);
    auto f_lineno = PyObject_GetAttrString(frame, "f_lineno");
    long lineno = PyLong_AsLong(f_lineno);
    Q_ASSERT(!PyErr_Occurred());

    // Clean up these objects immediately
    for (auto o : {inspect_module, frame, f_lineno})
        Py_DECREF(o);

    if (instructions.contains(lineno))
        throw hooks::HookException(
                "Cannot declare multiple UI elements on same line.");
    instructions.insert(lineno);

    return lineno;
}

QString ScriptUIHooks::getDatum(PyObject* obj)
{
    // Special case: None matches no datum.
    if (obj == Py_None)
        return QString();

    for (auto d : node->findChildren<Datum*>(
                QString(), Qt::FindDirectChildrenOnly))
        if (d->getValue() == obj)
            return d->objectName();
    return QString();
}

PyObject* ScriptUIHooks::tupleDragFunction(tuple t)
{
    if (len(t) != 3 && len(t) != 2)
        throw hooks::HookException("Must provide 2 or 3 arguments to drag tuple.");

    auto x = extract<object>(t[0])();
    auto xs = getDatum(x.ptr());

    auto y = extract<object>(t[1])();
    auto ys = getDatum(y.ptr());

    bool invalid_argument = (xs.isNull() && x.ptr() != Py_None) ||
                            (ys.isNull() && y.ptr() != Py_None);
    QString zs;
    if (len(t) == 3)
    {
        auto z = extract<object>(t[2])();
        zs = getDatum(z.ptr());
        invalid_argument |= (zs.isNull() && z.ptr() != Py_None);
    }

    if (invalid_argument)
        throw hooks::HookException(
                "Arguments to drag tuple must be None or datum values.");

    return defaultDragFunction(xs, ys, zs);
}

PyObject* ScriptUIHooks::defaultDragFunction(QString x, QString y, QString z)
{
    QString drag =
        "def drag(this, dx, dy, dz):\n"
        "    pass\n";
    if (!x.isNull())
        drag += QString("    this.%1 += dx\n").arg(x);
    if (!y.isNull())
        drag += QString("    this.%1 += dy\n").arg(y);
    if (!z.isNull())
        drag += QString("    this.%1 += dz\n").arg(z);

    auto globals = PyDict_New();
    PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());
    auto locals = Py_BuildValue("{}");
    auto out = PyRun_String(
            drag.toStdString().c_str(),
            Py_file_input, globals, locals);
    Q_ASSERT(!PyErr_Occurred());

    auto drag_func = PyDict_GetItemString(locals, "drag");
    Q_ASSERT(drag_func);
    Py_INCREF(drag_func);

    // Clean up references
    for (auto obj : {globals, locals, out})
        Py_DECREF(obj);

    return drag_func;
}


PyObject* ScriptUIHooks::getDragFunction(dict kwargs)
{
    if (kwargs.has_key("drag"))
    {
        // Generate tuple-style drag function if kwargs['drag'] is a tuple.
        auto t = extract<tuple>(kwargs["drag"]);
        if (t.check())
        {
            if (kwargs.has_key("relative"))
                throw hooks::HookException(
                        "Can't provide 'relative' argument "
                        "with tuple-style drag function");
            return tupleDragFunction(t());
        }
        else
        {
            // Otherwise, assume that kwargs['drag'] is callable and use it.
            auto d = extract<object>(kwargs["drag"])().ptr();
            Py_INCREF(d);
            return d;
        }
    }
    return NULL;
}

object ScriptUIHooks::point(tuple args, dict kwargs)
{
    ScriptUIHooks& self = extract<ScriptUIHooks&>(args[0])();

    // Find the instruction at which this callback happened
    // (used as a unique identifier for the Control).
    long lineno = self.getInstruction();

    if (len(args) != 4 && len(args) != 3)
        throw hooks::HookException("Expected x, y, z as arguments");

    // Extract x, y, z as floats from first three arguments.
    extract<float> x_(args[1]);
    if (!x_.check())
        throw hooks::HookException("x value must be a number");
    float x = x_();

    extract<float> y_(args[2]);
    if (!y_.check())
        throw hooks::HookException("y value must be a number");
    float y = y_();

    float z = 0;
    if (len(args) == 4)
    {
        extract<float> z_(args[3]);
        if (!z_.check())
            throw hooks::HookException("z value must be a number");
        z = z_();
    }

    ControlPoint* p = dynamic_cast<ControlPoint*>(
            self.scene->getControl(self.node, lineno));
    if (!p || p->isDeleteScheduled())
    {
        p = new ControlPoint(self.node);
        self.scene->registerControl(self.node, lineno, p);
    }

    const float r = getFloat(p->getR(), kwargs, "r");
    const QColor color = getColor(p->getColor(), kwargs);
    const bool relative = getBool(p->getRelative(), kwargs, "relative");

    PyObject* drag_func = self.getDragFunction(kwargs);
    // If we didn't get a drag function from the kwarg dict,
    // make a default drag function from datum matching.
    if (!drag_func)
    {
        if (kwargs.has_key("relative"))
            throw hooks::HookException(
                    "Can't provide 'relative' argument "
                    "without drag function");

        // Try to automatically generate a drag function by looking to see
        // if the x, y, z arguments match datum values; if so, make a drag
        // function that drags these datums.
        if (len(args) == 4)
            drag_func = defaultDragFunction(
                    self.getDatum(extract<object>(args[1])().ptr()),
                    self.getDatum(extract<object>(args[2])().ptr()),
                    self.getDatum(extract<object>(args[3])().ptr()));
        else
            drag_func = defaultDragFunction(
                    self.getDatum(extract<object>(args[1])().ptr()),
                    self.getDatum(extract<object>(args[2])().ptr()),
                    QString());
    }

    p->update(x, y, z, r, color, relative, drag_func);
    p->touch();

    // Return None
    return object();
}

object ScriptUIHooks::wireframe(tuple args, dict kwargs)
{
    ScriptUIHooks& self = extract<ScriptUIHooks&>(args[0])();

    // Find the instruction at which this callback happened
    // (used as a unique identifier for the Control).
    long lineno = self.getInstruction();

    if (len(args) != 2)
        throw hooks::HookException("Expected list of 3-tuples as argument");

    auto v = extractVectors(extract<object>(args[1])());
    if (v.isEmpty())
        throw hooks::HookException("Wireframe must have at least one point");

    ControlWireframe* w = dynamic_cast<ControlWireframe*>(
            self.scene->getControl(self.node, lineno));

    if (!w)
    {
        w = new ControlWireframe(self.node);
        self.scene->registerControl(self.node, lineno, w);
    }

    PyObject* drag_func = self.getDragFunction(kwargs);
    if (!drag_func && kwargs.has_key("relative"))
        throw hooks::HookException(
                "Can't provide 'relative' argument "
                "without drag function");

    const float t = getFloat(w->getT(), kwargs, "t");
    const QColor color = getColor(w->getColor(), kwargs);
    const bool close = getBool(w->getClose(), kwargs, "close");
    const bool relative = getBool(w->getRelative(), kwargs, "relative");
    w->update(v, t, color, close, relative, drag_func);
    w->touch();

    return object();
}

float ScriptUIHooks::getFloat(float v, dict kwargs, std::string key)
{
    if (kwargs.has_key(key))
    {
        extract<float> v_(kwargs[key]);
        if (!v_.check())
            throw hooks::HookException(key + " value must be a number");
        v = v_();
    }
    return v;
}

bool ScriptUIHooks::getBool(bool b, dict kwargs, std::string key)
{
    if (kwargs.has_key(key))
    {
        extract<bool> b_(kwargs[key]);
        if (!b_.check())
            throw hooks::HookException(key + " value must be a boolean");
        b = b_();
    }
    return b;
}

QColor ScriptUIHooks::getColor(QColor color, dict kwargs)
{
    if (kwargs.has_key("color"))
    {
        auto rgb = extractList<int>(kwargs["color"]);
        if (rgb.length() != 3)
            throw hooks::HookException("color tuple must have three values");
        color = QColor(rgb[0], rgb[1], rgb[2]);
    }
    return color;
}
