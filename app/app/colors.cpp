#include <Python.h>

#include <QVector>
#include <QPair>

#include "app/colors.h"

#include "graph/datum.h"
#include "fab/fab.h"

namespace Colors
{
QColor red("#ac4142");
QColor orange("#d28445");
QColor yellow("#f4bf75");
QColor green("#90a959");
QColor teal("#75b5aa");
QColor blue("#6a9fb5");
QColor violet("#aa759f");
QColor brown("#8f5536");

QColor base00("#151515");
QColor base01("#202020");
QColor base02("#303030");
QColor base03("#505050");
QColor base04("#b0b0b0");
QColor base05("#d0d0d0");
QColor base06("#e0e0e0");
QColor base07("#f5f5f5");

QColor adjust(QColor c, float scale)
{
    c.setRed(fmax(fmin(c.red()*scale, 255), 0));
    c.setGreen(fmax(fmin(c.green()*scale, 255), 0));
    c.setBlue(fmax(fmin(c.blue()*scale, 255), 0));
    return c;
}

QColor highlight(QColor c)
{
    return adjust(c, 1.4);
}

QColor dim(QColor c)
{
    return adjust(c, 1/1.4);
}

QColor getColor(const Datum *d)
{
    auto t = d->getType();
    if (t == &PyUnicode_Type)
        return brown;
    else if (t == &PyFloat_Type)
        return yellow;
    else if (t == &PyLong_Type)
        return orange;
    else if (t == fab::ShapeType)
        return green;
    else
        return red;
}

PyObject* PyColors()
{
    // Here are all of our standard colors and their names.
    static QVector<QPair<QString, QColor>> colors = {
        {"red", red},
        {"orange", orange},
        {"yellow", yellow},
        {"green", green},
        {"teal", teal},
        {"blue", blue},
        {"violet", violet},
        {"brown", brown},
        {"base00", base00},
        {"base01", base01},
        {"base02", base02},
        {"base03", base03},
        {"base04", base04},
        {"base05", base05},
        {"base06", base06},
        {"base07", base07}};

    // Lazy initialization of NamedTuple constructor
    static PyObject* colors_tuple = NULL;
    if (colors_tuple == NULL)
    {
        PyObject* tuple_constructor;

        // Build a namedtuple constructor that has all of the colors
        // as arguments.
        {
            auto list = PyList_New(colors.size());
            size_t i=0;
            for (auto c : colors)
                PyList_SetItem(list, i++, Py_BuildValue(
                            "s", c.first.toStdString().c_str()));

            auto collections = PyImport_ImportModule("collections");
            auto nt = PyObject_GetAttrString(collections, "namedtuple");
            auto args = Py_BuildValue("(sO)", "SbColors", list);
            tuple_constructor = PyObject_Call(nt, args, NULL);

            for (auto o : {collections, nt, args, list})
                Py_DECREF(o);
        }

        // Then, call this constructor on a list of color tuples.
        {
            auto list = PyList_New(colors.size());
            size_t i=0;
            for (auto c : colors)
                PyList_SetItem(list, i++, Py_BuildValue(
                        "(iii)", c.second.red(),
                        c.second.green(), c.second.blue()));
            auto args = PyList_AsTuple(list);
            colors_tuple = PyObject_Call(tuple_constructor, args, NULL);

            for (auto o : {list, args})
                Py_DECREF(o);
        }
        Q_ASSERT(!PyErr_Occurred());
    }

    return colors_tuple;
}

} // end of Colors namespace
