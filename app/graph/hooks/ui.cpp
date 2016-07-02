#include <boost/python.hpp>

#include <QVector>

#include "graph/hooks/ui.h"
#include "graph/hooks/hooks.h"
#include "graph/proxy/node.h"

#include "graph/node.h"

#include "viewport/control/point.h"
#include "viewport/control/wireframe.h"

using namespace boost::python;

////////////////////////////////////////////////////////////////////////////////

/*
 *  Extracts as tuple or list to a list of the given object type.
 *  Throws a HookException if any single extract() call fails.
 */
template <typename T, typename O>
static QVector<T> extractListOrTuple(O obj)
{
    QVector<T> out;
    for (int i=0; i < len(obj); ++i)
    {
        extract<T> e(obj[i]);
        if (!e.check())
            throw AppHooks::Exception(
                    "Failed to extract data from object.");
        out << e();
    }
    return out;
}

/*
 *  Extracts a tuple or list to a list of the given object type.
 *  Throws a HookException if the input is not a list or tuple, or if
 *  any single extraction fails.
 */
template <typename T>
static QVector<T> extractList(object obj)
{
    auto tuple_ = extract<tuple>(obj);
    if (tuple_.check())
        return extractListOrTuple<T>(tuple_());

    auto list_ = extract<list>(obj);
    if (list_.check())
        return extractListOrTuple<T>(list_());

    throw AppHooks::Exception("Input must be a list or a tuple");
}

////////////////////////////////////////////////////////////////////////////////

/*
 *  Converts a (list|tuple) of 3-element (lists|tuples) to vectors.
 *  Throws a HookException on failure.
 */
static QVector<QVector3D> extractVectors(object obj)
{
    QVector<QVector3D> out;

    // Try to extract a bunch of tuples from the top-level list.
    QVector<tuple> tuples;
    bool got_tuple = true;
    try {
        tuples = extractList<tuple>(obj);
    } catch (AppHooks::Exception e) {
        got_tuple = false;
    }

    if (got_tuple)
    {
        for (auto t : tuples)
        {
            auto v = extractList<float>(extract<object>(t)());
            if (v.length() != 3)
                throw AppHooks::Exception("Position data must have three terms.");
            out << QVector3D(v[0], v[1], v[2]);
        }
        return out;
    }

    // Try to extract a bunch of lists from the top-level list.
    QVector<list> lists;
    bool got_list = true;
    try {
        lists = extractList<list>(obj);
    } catch (AppHooks::Exception e) {
        got_list = false;
    }

    if (got_list)
    {
        for (auto l : lists)
        {
            auto v = extractList<float>(extract<object>(l)());
            if (v.length() != 3)
                throw AppHooks::Exception("Position data must have three terms.");
            out << QVector3D(v[0], v[1], v[2]);
        }
        return out;
    }

    throw AppHooks::Exception(
            "Position data must be a list of 3-element lists");
}

////////////////////////////////////////////////////////////////////////////////

/*
 *  Tries to extract a boolean value from kwargs.
 *  Returns b if no value with that key exists;
 *  throws hooks::HookException if the value is there but not a bool.
 */
static bool getBool(bool b, boost::python::dict kwargs, std::string key)
{
    if (kwargs.has_key(key))
    {
        extract<bool> b_(kwargs[key]);
        if (!b_.check())
            throw AppHooks::Exception(key + " value must be a boolean");
        b = b_();
    }
    return b;
}

/*
 *  Tries to extract an RGB color tuple from kwargs.
 *  Returns color if no such argument exists;
 *  throws a hooks::HookException if the argument exists but is
 *  incorrectly constructed.
 */
static QColor getColor(QColor color, boost::python::dict kwargs)
{
    if (kwargs.has_key("color"))
    {
        auto rgb = extractList<int>(kwargs["color"]);
        if (rgb.length() != 3)
            throw AppHooks::Exception("color tuple must have three values");
        color = QColor(rgb[0], rgb[1], rgb[2]);
    }
    return color;
}

/*
 *  Tries to get a particular value from kwargs.
 *  Returns v if no such value exists;
 *  throws hooks::HookException if the value is there but not a float.
 */
static float getFloat(float v, boost::python::dict kwargs, std::string key)
{
    if (kwargs.has_key(key))
    {
        extract<float> v_(kwargs[key]);
        if (!v_.check())
            throw AppHooks::Exception(key + " value must be a number");
        v = v_();
    }
    return v;
}

////////////////////////////////////////////////////////////////////////////////

static PyObject* defaultDragFunction(QString x, QString y, QString z)
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
                throw AppHooks::Exception(
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

PyObject* ScriptUIHooks::tupleDragFunction(tuple t)
{
    if (len(t) != 3 && len(t) != 2)
        throw AppHooks::Exception("Must provide 2 or 3 arguments to drag tuple.");

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
        throw AppHooks::Exception(
                "Arguments to drag tuple must be None or datum values.");

    return defaultDragFunction(xs, ys, zs);
}

////////////////////////////////////////////////////////////////////////////////

QString ScriptUIHooks::getDatum(PyObject* obj)
{
    // Special case: None matches no datum.
    if (obj == Py_None)
    {
        return QString();
    }

    for (auto d : proxy->getNode()->childDatums())
    {
        if (d->currentValue() == obj)
        {
            return QString::fromStdString(d->getName());
        }
    }
    return QString();
}

////////////////////////////////////////////////////////////////////////////////

Py_hash_t ScriptUIHooks::getKey(dict kwargs, std::string type)
{
    Py_hash_t key = -1;

    if (kwargs.has_key("key"))
    {
        auto key_str = PyObject_Str(extract<object>(kwargs["key"])().ptr());
        if (!key_str)
        {
            throw AppHooks::Exception("Key must be convertable to a string.");
        }
        Py_XDECREF(key_str);

        auto prefix = PyUnicode_FromString(("__KEY__" + type).c_str());
        auto target = PyUnicode_Concat(prefix, key_str);

        key = PyObject_Hash(target);
        Py_DECREF(prefix);
        Py_DECREF(target);

        if (keys.contains(key))
        {
            throw AppHooks::Exception(
                    "Collision with user-defined keys");
        }
    }
    else
    {
        // Get the current line and hash it
        // (used to uniquely identify calls to this function)
        auto inspect_module = PyImport_ImportModule("inspect");
        auto frame = PyObject_CallMethod(inspect_module, "currentframe", NULL);
        auto f_lineno = PyObject_GetAttrString(frame, "f_lineno");
        auto f_lineno_str = PyObject_Str(f_lineno);
        auto prefix = PyUnicode_FromString(("__LINE__" + type).c_str());
        auto target = PyUnicode_Concat(prefix, f_lineno_str);
        key = PyObject_Hash(target);
        Q_ASSERT(!PyErr_Occurred());

        // Clean up PyObjects immediately
        for (auto o : {inspect_module, frame, f_lineno,
                       f_lineno_str, prefix, target})
        {
            Py_DECREF(o);
        }

        if (keys.contains(key))
        {
            throw AppHooks::Exception(
                    "Cannot declare multiple UI elements on same line "
                    "(without unique keys)");
        }
    }

    keys.insert(key);
    return key;
}

////////////////////////////////////////////////////////////////////////////////

object ScriptUIHooks::wireframe(tuple args, dict kwargs)
{
    ScriptUIHooks& self = extract<ScriptUIHooks&>(args[0])();

    // Find the instruction at which this callback happened
    // (used as a unique identifier for the Control).
    Py_hash_t key = self.getKey(kwargs, "wireframe");

    if (len(args) != 2)
    {
        throw AppHooks::Exception("Expected list of 3-tuples as argument");
    }

    auto v = extractVectors(extract<object>(args[1])());
    if (v.isEmpty())
    {
        throw AppHooks::Exception("Wireframe must have at least one point");
    }

    WireframeControl* w = dynamic_cast<WireframeControl*>(
            self.proxy->getControl(key));

    if (!w)
    {
        w = new WireframeControl(self.proxy);
        self.proxy->registerControl(key, w);
    }

    PyObject* drag_func = self.getDragFunction(kwargs);
    if (!drag_func && kwargs.has_key("relative"))
    {
        throw AppHooks::Exception(
                "Can't provide 'relative' argument "
                "without drag function");
    }

    const float t = getFloat(w->t, kwargs, "t");
    const QColor color = getColor(w->color, kwargs);
    const bool close = getBool(w->close, kwargs, "close");
    const bool relative = getBool(w->relative, kwargs, "relative");

    w->update(v, t, color, close, relative, drag_func);
    w->touched = true;

    return object();
}

object ScriptUIHooks::point(tuple args, dict kwargs)
{
    ScriptUIHooks& self = extract<ScriptUIHooks&>(args[0])();

    // Find the instruction at which this callback happened
    // (used as a unique identifier for the Control).
    Py_hash_t key = self.getKey(kwargs, "point");

    if (len(args) != 4 && len(args) != 3)
    {
        throw AppHooks::Exception("Expected x, y, z as arguments");
    }

    // Extract x, y, z as floats from first three arguments.
    extract<float> x_(args[1]);
    if (!x_.check())
    {
        throw AppHooks::Exception("x value must be a number");
    }
    float x = x_();

    extract<float> y_(args[2]);
    if (!y_.check())
    {
        throw AppHooks::Exception("y value must be a number");
    }
    float y = y_();

    float z = 0;
    if (len(args) == 4)
    {
        extract<float> z_(args[3]);
        if (!z_.check())
        {
            throw AppHooks::Exception("z value must be a number");
        }
        z = z_();
    }

    PointControl* p = dynamic_cast<PointControl*>(
            self.proxy->getControl(key));
    if (!p)
    {
        p = new PointControl(self.proxy);
        self.proxy->registerControl(key, p);
    }

    const float r = getFloat(p->r, kwargs, "r");
    const QColor color = getColor(p->color, kwargs);
    const bool relative = getBool(p->relative, kwargs, "relative");

    PyObject* drag_func = self.getDragFunction(kwargs);

    // If we didn't get a drag function from the kwarg dict,
    // make a default drag function from datum matching.
    if (!drag_func)
    {
        if (kwargs.has_key("relative"))
        {
            throw AppHooks::Exception(
                    "Can't provide 'relative' argument "
                    "without drag function");
        }

        // Try to automatically generate a drag function by looking to see
        // if the x, y, z arguments match datum values; if so, make a drag
        // function that drags these datums.
        if (len(args) == 4)
        {
            drag_func = defaultDragFunction(
                    self.getDatum(extract<object>(args[1])().ptr()),
                    self.getDatum(extract<object>(args[2])().ptr()),
                    self.getDatum(extract<object>(args[3])().ptr()));
        }
        else
        {
            drag_func = defaultDragFunction(
                    self.getDatum(extract<object>(args[1])().ptr()),
                    self.getDatum(extract<object>(args[2])().ptr()),
                    QString());
        }
    }

    p->update(x, y, z, r, color, relative, drag_func);
    p->touched = true;

    // Return None
    return object();
}
