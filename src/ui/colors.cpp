#include <Python.h>
#include "ui/colors.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"
#include "datum/output_datum.h"
#include "datum/shape_datum.h"

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
    return adjust(c, 1.2);
}

QColor dim(QColor c)
{
    return adjust(c, 1/1.2);
}


QColor getColor(Datum *d)
{
    if (dynamic_cast<FloatDatum*>(d))
        return yellow;
    else if (dynamic_cast<ShapeDatum*>(d) ||
             dynamic_cast<ShapeFunctionDatum*>(d) ||
             dynamic_cast<ShapeOutputDatum*>(d))
        return blue;
    return red;
}

} // end of Colors namespace
