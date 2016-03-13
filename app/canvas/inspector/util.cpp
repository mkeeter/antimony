#include "canvas/inspector/util.h"

bool moveTo(QGraphicsItem* g, QPointF p)
{
    if (p != g->pos())
    {
        g->setPos(p);
        return true;
    }
    return false;
}
