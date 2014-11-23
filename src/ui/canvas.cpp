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

#include "graph/node/node.h"
#include "graph/datum/datum.h"
#include "graph/datum/link.h"

Canvas::Canvas(QWidget* parent)
    : QGraphicsView(parent)
{
    setStyleSheet("QGraphicsView { border-style: none; }");
    setRenderHints(QPainter::Antialiasing);
    setSceneRect(-width()/2, -height()/2, width(), height());

    QGLFormat format;
    format.setVersion(2, 1);
    format.setSampleBuffers(true);
    setViewport(new QGLWidget(format, this));
}

Canvas::Canvas(QGraphicsScene* s, QWidget* parent)
    : Canvas(parent)
{
    setScene(s);
}

void Canvas::setScene(QGraphicsScene* s)
{
    QGraphicsView::setScene(s);
    scene = s;

    scene->addItem(new QGraphicsRectItem(0, 0, 20, 20));
    scene->addItem(new QGraphicsRectItem(200, 0, 20, 20));
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);
    if (!event->isAccepted() && event->button() == Qt::LeftButton)
        click_pos = mapToScene(event->pos());
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
    if (scene->mouseGrabberItem() == NULL && event->buttons() == Qt::LeftButton)
    {
        auto d = click_pos - mapToScene(event->pos());
        setSceneRect(sceneRect().translated(d.x(), d.y()));
    }
}


#if 0
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

InputPort* Canvas::getInputPortNear(QPointF pos, Link* link) const
{
    float distance = INFINITY;
    InputPort* port = NULL;

    for (auto i : scene->items())
    {
        InputPort* p = dynamic_cast<InputPort*>(i);
        if (p && (link == NULL || p->getDatum()->acceptsLink(link)))
        {
            QPointF delta = p->mapToScene(p->boundingRect().center()) - pos;
            float d = QPointF::dotProduct(delta, delta);
            if (d < distance)
            {
                distance = d;
                port = p;
            }
        }
    }

    return port;
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
#endif

void Canvas::keyPressEvent(QKeyEvent *event)
{
    QGraphicsView::keyPressEvent(event);
    if (event->isAccepted())
        return;
#if 0
    if (event->key() == Qt::Key_A &&
                (event->modifiers() & Qt::ShiftModifier))
    {
        QMenu* m = new QMenu(this);

        auto window = dynamic_cast<MainWindow*>(parent()->parent());
        Q_ASSERT(window);
        window->populateMenu(m, false);

        m->exec(QCursor::pos());
        m->deleteLater();
    }
#endif
}

void Canvas::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->setBrush(QColor(200, 200, 200));
    painter->drawRect(rect);
}
