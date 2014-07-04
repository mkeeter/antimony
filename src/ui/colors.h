#ifndef COLORS_H
#define COLORS_H

#include <QColor>

class Datum;

namespace Colors
{
extern QColor yellow;
extern QColor violet;
extern QColor magenta;
extern QColor blue;
extern QColor base01;
extern QColor base00;
extern QColor base03;
extern QColor base02;
extern QColor orange;
extern QColor red;
extern QColor cyan;
extern QColor base0;
extern QColor base1;
extern QColor base2;
extern QColor base3;
extern QColor green;

QColor highlight(QColor c);
QColor dim(QColor c);
QColor getColor(Datum* d);
}

#endif // COLORS_H
