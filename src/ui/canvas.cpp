#include <Python.h>

#include <QMouseEvent>
#include <QDebug>
#include <QGridLayout>

#include <cmath>

#include "ui/canvas.h"
#include "ui/port.h"
#include "ui/connection.h"
#include "ui/inspector.h"
#include "ui/depth_image.h"

#include "node/node.h"
#include "datum/link.h"

#include "control/control.h"
#include "control/axes_control.h"

#include "datum/datum.h"

Canvas::Canvas(QWidget* parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(parent)),
      scale(100), pitch(0), yaw(0)
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
    M.rotate(pitch * 180 / M_PI, QVector3D(1, 0, 0));
    M.rotate(yaw  *  180 / M_PI, QVector3D(0, 0, 1));

    return M;
}

QMatrix4x4 Canvas::getMatrix2D() const
{
    QMatrix4x4 M;

    // Remember that these operations are applied in reverse order.
    M.scale(scale, -scale*cos(pitch), scale);
    M.rotate(yaw  *  180 / M_PI, QVector3D(0, 0, 1));

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

Control* Canvas::getControl(Node* node) const
{
    for (auto i : items())
    {
        Control* c = dynamic_cast<Control*>(i);

        if (c && c->getNode() == node && (c->parentItem() == NULL ||
            dynamic_cast<Control*>(c->parentItem())->getNode() != node))
        {
            return c;
        }
    }
    return NULL;
}

InputPort* Canvas::getInputPortAt(QPointF pos) const
{
    for (auto i : scene->items(pos))
    {
        InputPort* p = dynamic_cast<InputPort*>(i);
        if (p)
        {
            return p;
        }
    }
    return NULL;
}

NodeInspector* Canvas::getInspectorAt(QPointF pos) const
{
    for (auto i : scene->items(pos))
    {
        NodeInspector* p = dynamic_cast<NodeInspector*>(i);
        if (p)
        {
            return p;
        }
    }
    return NULL;
}

float Canvas::getZmax() const
{
    float zmax = -INFINITY;
    for (auto i : scene->items())
    {
        DepthImageItem* p = dynamic_cast<DepthImageItem*>(i);
        if (p)
        {
            zmax = fmax(zmax, p->zmax);
        }
    }
    return zmax;
}

float Canvas::getZmin() const
{
    float zmin = INFINITY;
    for (auto i : scene->items())
    {
        DepthImageItem* p = dynamic_cast<DepthImageItem*>(i);
        if (p)
        {
            zmin = fmin(zmin, p->zmin);
        }
    }
    return zmin;
}


void Canvas::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
    if (!event->isAccepted())
    {
        if (event->button() == Qt::LeftButton)
        {
            _click_pos = mapToScene(event->pos());
        }
        else
        {
            _click_pos = event->pos();
        }
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
    if (scene->mouseGrabberItem() == NULL)
    {
        if (event->buttons() == Qt::LeftButton)
        {
            pan(_click_pos - mapToScene(event->pos()));
        }
        else if (event->buttons() == Qt::RightButton)
        {
            QPointF d = _click_pos - event->pos();
            pitch = fmin(0, fmax(-M_PI, pitch - 0.01 * d.y()));
            yaw = fmod(yaw + M_PI - 0.01 * d.x(), M_PI*2) - M_PI;

            _click_pos = event->pos();
            update();
            emit(viewChanged());
        }
    }
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    QVector3D a = sceneToWorld(mapToScene(event->pos()));
    scale *= pow(1.001, -event->delta());
    QVector3D b = sceneToWorld(mapToScene(event->pos()));
    pan(worldToScene(a - b));
    emit(viewChanged());
}

void Canvas::keyPressEvent(QKeyEvent *event)
{
    if (scene->focusItem())
    {
        QGraphicsView::keyPressEvent(event);
    }
    else if (event->key() == Qt::Key_Delete)
    {
        for (auto i : scene->selectedItems())
        {
            Control* control = dynamic_cast<Control*>(i);
            Connection* conn = dynamic_cast<Connection*>(i);
            if (control)
            {
                control->deleteNode();
            }
            else if (conn)
            {
                conn->getLink()->deleteLater();
            }
        }
    }
}

void Canvas::pan(QPointF d)
{
    setSceneRect(sceneRect().translated(d));
}

#include <iostream>
#include <QTime>

void Canvas::paintEvent(QPaintEvent *event)
{
    QTime timer;
    timer.start();
    QGraphicsView::paintEvent(event);
    //std::cout << timer.elapsed() << std::endl;
}
