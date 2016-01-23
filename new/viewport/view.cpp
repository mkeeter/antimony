#include <QMatrix4x4>

#include "viewport/view.h"

ViewportView::ViewportView(QWidget* parent)
    : QGraphicsView(new QGraphicsScene(), parent)
{
    setStyleSheet("QGraphicsView { border-style: none; }");
    setRenderHints(QPainter::Antialiasing);

    QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QAbstractScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

QMatrix4x4 ViewportView::getMatrix() const
{
    return QMatrix4x4();
}
