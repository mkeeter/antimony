#include <Python.h>

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QMenu>
#include <QPropertyAnimation>

#include "viewport/view.h"
#include "viewport/scene.h"
#include "viewport/image.h"
#include "viewport/control/control.h"
#include "viewport/control/control_instance.h"

#include "app/colors.h"

#include "graph/proxy/datum.h"
#include "graph/constructor/populate.h"

ViewportView::ViewportView(QWidget* parent, ViewportScene* scene)
    : QGraphicsView(new QGraphicsScene(), parent), gl(new QOpenGLWidget(this)),
      scale(100), pitch(0), yaw(0), view_scene(scene)
{
    setStyleSheet("QGraphicsView { border-style: none; }");
    setRenderHints(QPainter::Antialiasing);

    setViewport(gl.context);

    setSceneRect(-width()/2, -height()/2, width(), height());
    setMouseTracking(true);

    QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QAbstractScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

////////////////////////////////////////////////////////////////////////////////

QMatrix4x4 ViewportView::getMatrix(int params) const
{
    QMatrix4x4 M;

    // Remember that these operations are applied in reverse order.
    if (params & SCALE)
    {
        M.scale(scale, -scale, scale);
    }

    if (params & ROT)
    {
        M.rotate(pitch * 180 / M_PI, QVector3D(1, 0, 0));
        M.rotate(yaw  *  180 / M_PI, QVector3D(0, 0, 1));
    }

    if (params & MOVE)
    {
        M.translate(-center.x(), -center.y(), -center.z());
    }

    return M;
}

QVector3D ViewportView::sceneToWorld(QPointF p) const
{
    return getMatrix().inverted() * QVector3D(p.x(), p.y(), 0);
}

void ViewportView::installImage(DepthImage* d)
{
    connect(this, &ViewportView::getDepth,
            d, &DepthImage::getDepth);
    connect(this, &ViewportView::paintImage,
            d, &DepthImage::paint);
}

////////////////////////////////////////////////////////////////////////////////

void ViewportView::lockAngle(float y, float p)
{
    yaw = y;
    pitch = p;
    angle_locked = true;
}

////////////////////////////////////////////////////////////////////////////////

void ViewportView::drawBackground(QPainter* painter, const QRectF& rect)
{
    QGraphicsView::drawBackground(painter, rect);

    painter->beginNativePainting();
    if (!gl_initialized)
    {
        initializeOpenGLFunctions();
        gl_initialized = true;
    }
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get bounds from all child images
    float zmin = INFINITY;
    float zmax = -INFINITY;
    auto m = getMatrix();
    emit(getDepth(m, &zmin, &zmax));

    // Paint all images
    emit(paintImage(m, zmin, zmax));

    painter->endNativePainting();
}

void ViewportView::drawAxes(QPainter* painter) const
{
    // First, draw the axes.
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

void ViewportView::drawCoords(QPainter* painter) const
{
    QPointF mouse_pos = mapToScene(mapFromGlobal(QCursor::pos()));
    if (!sceneRect().contains(mouse_pos))
    {
        return;
    }

    // Get rotate-only transform matrix
    QMatrix4x4 M = getMatrix(ROT);
    const float threshold = 0.98;

    const auto a = M.inverted() * QVector3D(0, 0, 1);

    QList<QPair<char, QVector3D>> axes = {
        {'x', QVector3D(1, 0, 0)},
        {'y', QVector3D(0, 1, 0)},
        {'z', QVector3D(0, 0, 1)}};

    char axis = 0;
    float opacity = 0;
    for (const auto v : axes)
    {
        float dot = fabs(QVector3D::dotProduct(a, v.second));
        if (dot > threshold)
        {
            axis = v.first;
            opacity = (dot - threshold) / (1 - threshold);
        }
    }

    auto p = sceneToWorld(mouse_pos);

    int value = opacity * 200;

    painter->setPen(QPen(QColor(255, 255, 255, value)));
    QString txt;
    if (axis == 'z')
    {
        txt = QString("X: %1\nY: %2").arg(p.x()).arg(p.y());
    }
    else if (axis == 'y')
    {
        txt = QString("X: %1\nZ: %2").arg(p.x()).arg(p.z());
    }
    else if (axis == 'x')
    {
        txt = QString("Y: %1\nZ: %2").arg(p.y()).arg(p.z());
    }

    painter->drawText({-width()/2.0 + 10, -height()/2.0 + 10, 300, 200}, txt);
}

void ViewportView::drawForeground(QPainter* painter, const QRectF& rect)
{
    QGraphicsView::drawForeground(painter, rect);
    drawAxes(painter);
    drawCoords(painter);
}

////////////////////////////////////////////////////////////////////////////////

void ViewportView::update()
{
    scene()->invalidate();
    emit(changed(getMatrix(), geometry()));

    emit(centerChanged(center));
    emit(scaleChanged(scale));
}

////////////////////////////////////////////////////////////////////////////////

void ViewportView::installControl(Control* c)
{
    c->makeInstanceFor(this);
}

void ViewportView::installDatum(BaseDatumProxy* d)
{
    d->addViewport(this);
}

////////////////////////////////////////////////////////////////////////////////

void ViewportView::setScale(float s)
{
    scale = s;

    // Update the view without calling ViewportView::update
    // (because that would lead to infinite recursion)
    scene()->invalidate();
    emit(changed(getMatrix(), geometry()));
}

void ViewportView::setCenter(QVector3D c)
{
    center = c;

    // Update the view without calling ViewportView::update
    // (because that would lead to infinite recursion)
    scene()->invalidate();
    emit(changed(getMatrix(), geometry()));
}

////////////////////////////////////////////////////////////////////////////////

void ViewportView::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);

    // If the event hasn't been accepted, record click position for
    // panning / rotation on mouse drag.
    if (!event->isAccepted())
    {
        if (event->button() == Qt::LeftButton)
        {
            click_pos = mapToScene(event->pos());
            click_pos_world = sceneToWorld(click_pos);
        }
        else
        {
            click_pos = event->pos();
        }
        dragged = false;
    }
}

void ViewportView::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);

    current_pos = event->pos();
    if (scene()->mouseGrabberItem() == NULL)
    {
        if (event->buttons() == Qt::LeftButton)
        {
            center += click_pos_world - sceneToWorld(mapToScene(event->pos()));
            update();
        }
        else if (event->buttons() == Qt::RightButton && !angle_locked)
        {
            QPointF d = click_pos - event->pos();
            pitch = fmin(0, fmax(-M_PI, pitch - 0.01 * d.y()));
            yaw = fmod(yaw + M_PI - 0.01 * d.x(), M_PI*2) - M_PI;

            click_pos = event->pos();
            update();
        }
        dragged = true;
    }

    // Redraw to update cursor position
    scene()->invalidate(QRect(), QGraphicsScene::ForegroundLayer);
}

void ViewportView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton && !dragged)
    {
        auto is = items(event->pos());
        if (is.size() > 1)
        {
            openRaiseMenu(is);
        }
        else if (is.size())
        {
            if (auto c = dynamic_cast<ControlInstance*>(is.front()))
            {
                c->openContextMenu();
            }
        }
        else
        {
            openAddMenu(true);
        }
    }
    else
    {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void ViewportView::wheelEvent(QWheelEvent* event)
{
    QVector3D a = sceneToWorld(mapToScene(event->pos()));
    scale *= pow(1.001, -event->delta());
    QVector3D b = sceneToWorld(mapToScene(event->pos()));
    center += a - b;
    update();
}

void ViewportView::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    setSceneRect(-width()/2, -height()/2, width(), height());
}

void ViewportView::keyPressEvent(QKeyEvent* event)
{
    QGraphicsView::keyPressEvent(event);
    if (event->isAccepted())
    {
        return;
    }
    if (event->key() == Qt::Key_A &&
        (event->modifiers() & Qt::ShiftModifier))
    {
        openAddMenu();
    }
}

////////////////////////////////////////////////////////////////////////////////

void ViewportView::openAddMenu(bool view_commands)
{
    QMenu* m = new QMenu(this);

    if (view_commands && !angle_locked)
    {
        auto sub = new QMenu("View");

        connect(sub->addAction("Top"), &QAction::triggered,
                [=]{ this->spinTo(0, 0); });
        connect(sub->addAction("Bottom"), &QAction::triggered,
                [=]{ this->spinTo(0, -M_PI); });
        connect(sub->addAction("Left"), &QAction::triggered,
                [=]{ this->spinTo(M_PI/2, -M_PI/2); });
        connect(sub->addAction("Right"), &QAction::triggered,
                [=]{ this->spinTo(-M_PI/2, -M_PI/2); });
        connect(sub->addAction("Front"), &QAction::triggered,
                [=]{ this->spinTo(0, -M_PI/2); });
        connect(sub->addAction("Back"), &QAction::triggered,
                [=]{ this->spinTo(-M_PI, -M_PI/2); });

        m->addMenu(sub);
        m->addSeparator();
    }

    populateNodeMenu(m, view_scene->getGraph());

    m->exec(QCursor::pos());
    m->deleteLater();
}

void ViewportView::openRaiseMenu(QList<QGraphicsItem*> items)
{
    QScopedPointer<QMenu> m(new QMenu(this));

    int found = 0;
    for (auto i : items)
    {
        if (auto c = dynamic_cast<ControlInstance*>(i))
        {
            auto a = new QAction(c->getName(), m.data());
            a->setData(QVariant::fromValue(c));
            m->addAction(a);
            found++;
        }
    }

    QAction* chosen = (found > 1) ? m->exec(QCursor::pos())
                                  : NULL;

    if (chosen)
    {
        if (raised)
        {
            raised->setZValue(0);
        }
        raised = chosen->data().value<ControlInstance*>();
        raised->setZValue(0.1);
    }
}

////////////////////////////////////////////////////////////////////////////////

void ViewportView::setYaw(float y)
{
    yaw = y;
    update();
}

void ViewportView::setPitch(float p)
{
    pitch = p;
    update();
}

void ViewportView::spinTo(float new_yaw, float new_pitch)
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

////////////////////////////////////////////////////////////////////////////////

void ViewportView::zoomTo(Node* n)
{
    // Find all ControlInstances that are declared by this node
    QList<ControlInstance*> instances;
    for (auto i : items())
    {
        if (auto c = dynamic_cast<ControlInstance*>(i))
        {
            if (c->getNode() == n)
            {
                instances.push_back(c);
            }
        }
    }

    // Find a weighted sum of central points
    QVector3D pos;
    float area_sum = 0;
    for (auto i : instances)
    {
        const float area = i->boundingRect().width() *
                           i->boundingRect().height();
        pos += i->getControl()->pos() * area;
        area_sum += area;
    }
    pos /= area_sum;

    auto a = new QPropertyAnimation(this, "_center");
    a->setDuration(100);
    a->setStartValue(center);
    a->setEndValue(pos);

    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void ViewportView::hideUI(bool b)
{
    ui_hidden = b;
    for (auto i : scene()->items())
    {
        if (auto c = dynamic_cast<ControlInstance*>(i))
        {
            if (b)
            {
                c->hide();
            }
            else
            {
                c->show();
            }
        }
    }
}
