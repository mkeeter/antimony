#include <Python.h>

#include <QMouseEvent>
#include <QDebug>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QGLWidget>
#include <QMenu>

#include <cmath>

#include "ui/canvas.h"
#include "ui/port.h"
#include "ui/main_window.h"
#include "ui/connection.h"
#include "ui/inspector/inspector.h"
#include "ui/depth_image.h"

#include "node/node.h"
#include "datum/link.h"

#include "control/control.h"
#include "control/axes_control.h"
#include "ui/view_selector.h"

#include "datum/datum.h"

Canvas::Canvas(QWidget* parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(parent)),
      scale(100), pitch(0), yaw(0), view_selector(new ViewSelector(this)),
      ports_visible(true)
{
    setScene(scene);
    setStyleSheet("QGraphicsView { border-style: none; }");

    setSceneRect(-width()/2, -height()/2, width(), height());
    setRenderHints(QPainter::Antialiasing);

    QGLFormat format;
    format.setVersion(2, 1);
    format.setSampleBuffers(true);
    setViewport(new QGLWidget(format, this));

    new AxesControl(this);
}

void Canvas::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    view_selector->setPos(width()/2 - 70, -height()/2 + 70);
    setSceneRect(-width()/2, -height()/2, width(), height());
}

QMatrix4x4 Canvas::getMatrix() const
{
    QMatrix4x4 M;

    // Remember that these operations are applied in reverse order.
    M.scale(scale, -scale, scale);
    M.rotate(pitch * 180 / M_PI, QVector3D(1, 0, 0));
    M.rotate(yaw  *  180 / M_PI, QVector3D(0, 0, 1));
    M.translate(-center.x(), -center.y(), -center.z());

    return M;
}

QMatrix4x4 Canvas::getTransformMatrix() const
{
    QMatrix4x4 M;

    // Remember that these operations are applied in reverse order.
    M.rotate(pitch * 180 / M_PI, QVector3D(1, 0, 0));
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
            zmax = fmax(zmax, (getTransformMatrix() * p->pos).z() +
                                p->size.z()/2);
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
            zmin = fmin(zmin, (getTransformMatrix() * p->pos).z() -
                                p->size.z()/2);
        }
    }
    return zmin;
}

void Canvas::spinTo(float new_yaw, float new_pitch)
{
    QPropertyAnimation* a = new QPropertyAnimation(this, "_yaw", this);
    a->setDuration(100);
    a->setStartValue(yaw);
    a->setEndValue(new_yaw);

    QPropertyAnimation* b = new QPropertyAnimation(this, "_pitch", this);
    b->setDuration(100);
    b->setStartValue(pitch);
    b->setEndValue(new_pitch);

    a->start(QPropertyAnimation::DeleteWhenStopped);
    b->start(QPropertyAnimation::DeleteWhenStopped);
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
    // On right-click, show a menu of items to raise.
    if (event->button() == Qt::RightButton)
    {
        auto menu = new QMenu(this);
        QSet<Control*> used;
        for (auto i : items(event->pos()))
        {
            while (i->parentItem())
            {
                i = i->parentItem();
            }

            auto c = dynamic_cast<Control*>(i);

            if (c && !used.contains(c) && !dynamic_cast<AxesControl*>(i))
            {
                auto n = c->getNode();
                auto a = new QAction(
                        n->getName() + " (" + n->getType() + ")", menu);
                a->setData(QVariant::fromValue(i));
                menu->addAction(a);
                used << c;
            }
        }
        if (!menu->isEmpty())
        {
            QAction* chosen = menu->exec(QCursor::pos());
            if (chosen)
            {
                if (raised)
                {
                    raised->setZValue(1);
                }
                raised = static_cast<Control*>(
                        chosen->data().value<QGraphicsItem*>());
                raised->setZValue(1.1);
            }
        }
        menu->deleteLater();
    }

    QGraphicsView::mousePressEvent(event);
    if (!event->isAccepted())
    {
        if (event->button() == Qt::LeftButton)
        {
            _click_pos = mapToScene(event->pos());
            _click_pos_world = sceneToWorld(_click_pos);
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
            pan(_click_pos_world - sceneToWorld(mapToScene(event->pos())));
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

void Canvas::setYaw(float y)
{
    yaw = y;
    update();
    emit(viewChanged());
}

void Canvas::setPitch(float p)
{
    pitch = p;
    update();
    emit(viewChanged());
}

void Canvas::wheelEvent(QWheelEvent *event)
{
    QVector3D a = sceneToWorld(mapToScene(event->pos()));
    scale *= pow(1.001, -event->delta());
    QVector3D b = sceneToWorld(mapToScene(event->pos()));
    pan(a - b);
    emit(viewChanged());
}

void Canvas::keyPressEvent(QKeyEvent *event)
{
    if (scene->focusItem())
    {
        QGraphicsView::keyPressEvent(event);
    }
    else if (event->key() == Qt::Key_Space)
    {
        QGraphicsItem* i = scene->itemAt(
                mapToScene(mapFromGlobal(QCursor::pos())),
                QTransform());
        Control* control = dynamic_cast<Control*>(i);
        if (control)
        {
            control->toggleInspector();
        }
    }
    else if (event->key() == Qt::Key_Alt)
    {
        hideUI();
    }
    else if (event->key() == Qt::Key_Delete ||
             event->key() == Qt::Key_Backspace)
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
    else if (event->key() == Qt::Key_A &&
                (event->modifiers() & Qt::ShiftModifier))
    {
        QMenu* m = new QMenu(this);

        auto window = dynamic_cast<MainWindow*>(parent()->parent());
        Q_ASSERT(window);
        window->populateMenu(m, false);

        m->exec(QCursor::pos());
        m->deleteLater();
    }
    else if (event->key() == Qt::Key_S &&
                (event->modifiers() & Qt::ShiftModifier))
    {
        ports_visible = !ports_visible;
        emit(showPorts(ports_visible));
    }
}

void Canvas::keyReleaseEvent(QKeyEvent *event)
{
    if (scene->focusItem())
    {
        QGraphicsView::keyPressEvent(event);
    }
    else if (event->key() == Qt::Key_Alt)
    {
        showUI();
    }
}

void Canvas::hideUI()
{

    for (auto i : scene->items())
    {
        Control* control = dynamic_cast<Control*>(i);
        Connection* conn = dynamic_cast<Connection*>(i);
        AxesControl* ax = dynamic_cast<AxesControl*>(i);
        NodeInspector* inspector = dynamic_cast<NodeInspector*>(i);
        Port* p = dynamic_cast<Port*>(i);
        if (control && !ax)
            control->hide();
        else if (conn)
            conn->hide();
        else if (inspector)
            inspector->hide();
        else if (p)
            p->hide();
    }
}

void Canvas::showUI()
{

    for (auto i : scene->items())
    {
        Control* control = dynamic_cast<Control*>(i);
        Connection* conn = dynamic_cast<Connection*>(i);
        NodeInspector* inspector = dynamic_cast<NodeInspector*>(i);
        Port* p = dynamic_cast<Port*>(i);
        if (control)
            control->show();
        else if (conn)
            conn->show();
        else if (inspector)
            inspector->show();
        else if (p)
            p->show();
    }
}

void Canvas::pan(QVector3D d)
{
    center += d;
    update();
    emit(viewChanged());
}

#include <iostream>
#include <QTime>

void Canvas::drawBackground(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(painter);
    Q_UNUSED(rect);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Canvas::paintEvent(QPaintEvent *event)
{
    QTime timer;
    timer.start();
    QGraphicsView::paintEvent(event);
    //std::cout << timer.elapsed() << std::endl;
}
