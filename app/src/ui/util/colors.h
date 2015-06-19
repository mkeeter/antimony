#ifndef COLORS_H
#define COLORS_H

#include <QColor>

class Datum;

namespace Colors
{
extern QColor red;
extern QColor orange;
extern QColor yellow;
extern QColor green;
extern QColor teal;
extern QColor blue;
extern QColor violet;
extern QColor brown;

extern QColor base00;
extern QColor base01;
extern QColor base02;
extern QColor base03;
extern QColor base04;
extern QColor base05;
extern QColor base06;
extern QColor base07;

QColor adjust(QColor c, float scale);
QColor highlight(QColor c);
QColor dim(QColor c);
QColor getColor(Datum* d);

/*
 * Injects a 'colors' module into the fab namespace.
 */
void loadColors();
}

#endif // COLORS_H
