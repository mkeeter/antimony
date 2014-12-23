#include <Python.h>

#include <QMouseEvent>
#include <QDebug>
#include <QGLWidget>
#include <QMenu>

#include <cmath>

#include "ui/canvas/canvas.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/util/colors.h"

#include "graph/node/node.h"
#include "graph/datum/datum.h"
#include "graph/datum/link.h"

Canvas::Canvas(QWidget* parent)
    : QGraphicsView(parent)
{
    setStyleSheet("QGraphicsView { border-style: none; }");
    setRenderHints(QPainter::Antialiasing);
    setSceneRect(-width()/2, -height()/2, width(), height());

    QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QAbstractScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

Canvas::Canvas(QGraphicsScene* s, QWidget* parent)
    : Canvas(parent)
{
    QGraphicsView::setScene(s);
    scene = s;
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

void Canvas::wheelEvent(QWheelEvent* event)
{
    QPointF a = mapToScene(event->pos());
    auto s = pow(1.001, -event->delta());
    scale(s, s);
    auto d = a - mapToScene(event->pos());
    setSceneRect(sceneRect().translated(d.x(), d.y()));
}

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
    painter->setBrush(Colors::base00);
    painter->drawRect(rect);

    const int d = 20;
    painter->setPen(Colors::base03);
    for (int i = int(rect.left() / d) * d; i < rect.right(); i += d)
        for (int j = int(rect.top() / d) * d; j < rect.bottom(); j += d)
            painter->drawPoint(i, j);
}

NodeInspector* Canvas::getNodeInspector(Node* n) const
{
    for (auto i : items())
    {
        auto p = dynamic_cast<NodeInspector*>(i);
        if (p && p->getNode() == n)
            return p;
    }
    return NULL;
}
