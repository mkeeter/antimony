#include <Python.h>

#include <algorithm>

#include <QMouseEvent>
#include <QDebug>
#include <QGridLayout>
#include <QPropertyAnimation>
#include <QGLWidget>
#include <QMenu>

#include <cmath>

#include "ui/viewport/viewport.h"
#include "ui/viewport/depth_image.h"
#include "ui/viewport/view_selector.h"

#include "ui/main_window.h"
#include "ui/colors.h"

#include "graph/datum/datum.h"
#include "graph/node/node.h"
#include "graph/datum/link.h"

#include "control/control.h"
#include "control/axes_control.h"

Viewport::Viewport(QGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(parent), scene(scene),
      scale(100), pitch(0), yaw(0), view_selector(new ViewSelector(this))
{
    setScene(scene);
    setStyleSheet("QGraphicsView { border-style: none; }");

    setSceneRect(-width()/2, -height()/2, width(), height());
    setRenderHints(QPainter::Antialiasing);

    QGLFormat format;
    format.setVersion(2, 1);
    format.setSampleBuffers(true);
    setViewport(new QGLWidget(format, this));
#if 0
    new AxesControl(this);
#endif
}

void Viewport::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    view_selector->setPos(width()/2 - 70, -height()/2 + 70);
    setSceneRect(-width()/2, -height()/2, width(), height());
}

QMatrix4x4 Viewport::getMatrix() const
{
    QMatrix4x4 M;

    // Remember that these operations are applied in reverse order.
    M.scale(scale, -scale, scale);
    M.rotate(pitch * 180 / M_PI, QVector3D(1, 0, 0));
    M.rotate(yaw  *  180 / M_PI, QVector3D(0, 0, 1));
    M.translate(-center.x(), -center.y(), -center.z());

    return M;
}

QMatrix4x4 Viewport::getTransformMatrix() const
{
    QMatrix4x4 M;

    // Remember that these operations are applied in reverse order.
    M.rotate(pitch * 180 / M_PI, QVector3D(1, 0, 0));
    M.rotate(yaw  *  180 / M_PI, QVector3D(0, 0, 1));

    return M;
}


QPointF Viewport::worldToScene(QVector3D v) const
{
    QMatrix4x4 M = getMatrix();
    QVector3D w = M * v;
    return QPointF(w.x(), w.y());
}

QVector<QPointF> Viewport::worldToScene(QVector<QVector3D> v) const
{
    QVector<QPointF> out;
    for (auto p : v)
        out << worldToScene(p);
    return out;
}

QVector3D Viewport::sceneToWorld(QPointF p) const
{
    QMatrix4x4 M = getMatrix().inverted();
    return M * QVector3D(p.x(), p.y(), 0);
}

Control* Viewport::getControl(Node* node) const
{
#if 0
    for (auto i : items())
    {
        Control* c = dynamic_cast<Control*>(i);

        if (c && c->getNode() == node && (c->parentItem() == NULL ||
            dynamic_cast<Control*>(c->parentItem())->getNode() != node))
        {
            return c;
        }
    }
#endif
    return NULL;
}

float Viewport::getZmax() const
{
    float zmax = -INFINITY;
    for (auto i : scene->items())
    {
        if (DepthImageItem* p = dynamic_cast<DepthImageItem*>(i))
        {
            zmax = fmax(zmax, (getTransformMatrix() * p->pos).z() +
                                p->size.z()/2);
        }
    }
    return zmax;
}

float Viewport::getZmin() const
{
    float zmin = INFINITY;
    for (auto i : scene->items())
    {
        if (DepthImageItem* p = dynamic_cast<DepthImageItem*>(i))
        {
            zmin = fmin(zmin, (getTransformMatrix() * p->pos).z() -
                                p->size.z()/2);
        }
    }
    return zmin;
}

void Viewport::spinTo(float new_yaw, float new_pitch)
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

void Viewport::mousePressEvent(QMouseEvent *event)
{
    // On right-click, show a menu of items to raise.
    if (event->button() == Qt::RightButton)
    {
#if 0
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
#endif
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

void Viewport::mouseMoveEvent(QMouseEvent *event)
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
            scene->invalidate(QRect(),QGraphicsScene::ForegroundLayer);
            emit(viewChanged());
        }
    }
}

void Viewport::setYaw(float y)
{
    yaw = y;
    update();
    scene->invalidate(QRect(),QGraphicsScene::ForegroundLayer);
    emit(viewChanged());
}

void Viewport::setPitch(float p)
{
    pitch = p;
    update();
    scene->invalidate(QRect(),QGraphicsScene::ForegroundLayer);
    emit(viewChanged());
}

void Viewport::wheelEvent(QWheelEvent *event)
{
    QVector3D a = sceneToWorld(mapToScene(event->pos()));
    scale *= pow(1.001, -event->delta());
    QVector3D b = sceneToWorld(mapToScene(event->pos()));
    pan(a - b);
    emit(viewChanged());
}

void Viewport::keyPressEvent(QKeyEvent *event)
{
    QGraphicsView::keyPressEvent(event);
    if (event->isAccepted())
        return;

    if (event->key() == Qt::Key_Alt)
    {
        hideUI();
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
}

void Viewport::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsView::keyPressEvent(event);
    if (event->isAccepted())
        return;

    if (event->key() == Qt::Key_Alt)
        showUI();
}

void Viewport::hideUI()
{
#if 0
    for (auto i : scene->items())
    {
        Control* c = dynamic_cast<Control*>(i);
        if (c && !dynamic_cast<AxesControl*>(i))
            c->hide();
    }
#endif
}

void Viewport::showUI()
{
#if 0
    for (auto i : scene->items())
        if (Control* control = dynamic_cast<Control*>(i))
            control->show();
#endif
}

void Viewport::pan(QVector3D d)
{
    center += d;
    update();
    scene->invalidate(QRect(),QGraphicsScene::ForegroundLayer);
    emit(viewChanged());
}

void Viewport::drawBackground(QPainter* painter, const QRectF& rect)
{
    qDebug() << "drawn";
    Q_UNUSED(painter);
    Q_UNUSED(rect);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Viewport::drawForeground(QPainter* painter, const QRectF& rect)
{
    qDebug() << "f";
    Q_UNUSED(rect);

    auto m = getMatrix();
    QVector3D o = m * QVector3D(0, 0, 0);
    QVector3D x = m * QVector3D(1, 0, 0);
    QVector3D y = m * QVector3D(0, 1, 0);
    QVector3D z = m * QVector3D(0, 0, 1);

    QList<QPair<QVector3D, QColor>> pts = {
        {x, Colors::red},
        {y, Colors::green},
        {z, Colors::blue}};

    // Sort the axes to fake proper z clipping
    std::sort(pts.begin(), pts.end(),
            [](QPair<QVector3D, QColor> a, QPair<QVector3D, QColor> b)
            { return a.first.z() < b.first.z(); });

    for (auto p : pts)
    {
        painter->setPen(QPen(p.second, 2));
        painter->drawLine(o.toPointF(), p.first.toPointF());
    }
}
