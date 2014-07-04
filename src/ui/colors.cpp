#include <Python.h>
#include "ui/colors.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"
#include "datum/output_datum.h"
#include "datum/shape_datum.h"

namespace Colors
{
QColor yellow("#b58900");
QColor violet("#6c71c4");
QColor magenta("#d33682");
QColor blue("#268bd2");
QColor base01("#586e75");
QColor base00("#657b83");
QColor base03("#002b36");
QColor base02("#073642");
QColor orange("#cb4b16");
QColor red("#dc322f");
QColor cyan("#2aa198");
QColor base0("#839496");
QColor base1("#93a1a1");
QColor base2("#eee8d5");
QColor base3("#fdf6e3");
QColor green("#859900");

static QColor adjust(QColor c, float scale)
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

}
