#include <Python.h>

#include <QVector>
#include <QPair>

#include "ui/util/colors.h"
#include "graph/datum/datum.h"

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


QColor getColor(Datum *d)
{
    switch (d->getDatumType())
    {
        case DatumType::STRING: return brown;
        case DatumType::FLOAT:
        case DatumType::FLOAT_OUTPUT:
                                return yellow;
        case DatumType::INT:    return orange;
        case DatumType::SHAPE_INPUT:
        case DatumType::SHAPE_FUNCTION:
        case DatumType::SHAPE_OUTPUT:
        case DatumType::SHAPE:
                                return green;
        default:                return red;
    }
}

void loadColors()
{
    auto fab = PyImport_ImportModule("fab");
    auto color_module = PyModule_New("color");

    QVector<QPair<QString, QColor>> colors = {
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

    for (auto color : colors)
    {
        auto c = Py_BuildValue(
                "(iii)", color.second.red(),
                color.second.green(), color.second.blue());
        PyObject_SetAttrString(
                color_module, color.first.toStdString().c_str(), c);
        Py_DECREF(c);
    }

    PyObject_SetAttrString(fab, "color", color_module);
    Py_DECREF(fab);
    Py_DECREF(color_module);

    Q_ASSERT(!PyErr_Occurred());
}

} // end of Colors namespace
