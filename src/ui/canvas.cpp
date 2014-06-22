#include <QMouseEvent>

#include "canvas.h"
#include "control/axes_control.h"

Canvas::Canvas(QWidget* parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(parent)),
      scale(1), pitch(0), yaw(0)
{
    setScene(scene);
    setStyleSheet("QGraphicsView { border-style: none; }");
    setBackgroundBrush(Qt::black);

    setSceneRect(-width()/2, -height()/2, width(), height());
    setRenderHints(QPainter::Antialiasing);

    new AxesControl(this);
}

QMatrix4x4 Canvas::getMatrix() const
{
    QMatrix4x4 M;

    // Remember that these operations are applied in reverse order.
    M.scale(scale, -scale, scale);
    M.rotate(pitch, QVector3D(1, 0, 0));
    M.rotate(yaw,   QVector3D(0, 0, 1));

    return M;
}

QPointF Canvas::worldToScene(QVector3D v) const
{
    QMatrix4x4 M = getMatrix();
    QVector3D w = M * v;
    return QPointF(w.x(), w.y());
}

QVector<QPointF> Canvas::worldToScene(QVector<QVector3D> v) const
{
    QVector<QPointF> out;
    for (auto p : v)
    {
        out << worldToScene(p);
    }
    return out;
}

QVector3D Canvas::sceneToWorld(QPointF p) const
{
    QMatrix4x4 M = getMatrix().inverted();
    return M * QVector3D(p.x(), p.y(), 0);
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    // bla bla bla save mouse position here
}

void Canvas::pan(QPointF d)
{
    setSceneRect(sceneRect().translated(d));
}
