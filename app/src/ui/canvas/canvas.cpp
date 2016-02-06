#include <Python.h>

#include <QMouseEvent>
#include <QDebug>
#include <QMenu>
#include <QClipboard>
#include <QMimeData>
#include <QJsonDocument>
#include <QPropertyAnimation>

#include <cmath>

#include "ui/canvas/canvas.h"
#include "ui/canvas/graph_scene.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/port.h"
#include "ui/canvas/connection.h"
#include "ui/util/colors.h"
#include "ui/main_window.h"
#include "ui_main_window.h"

#include "graph/node.h"
#include "graph/graph.h"
#include "graph/datum.h"

#include "app/app.h"
#include "app/undo/undo_add_node.h"
#include "app/undo/undo_add_multi.h"
#include "app/undo/undo_delete_multi.h"

#include "graph/node/serializer.h"
#include "graph/node/deserializer.h"

Canvas::Canvas(QWidget* parent)
    : QGraphicsView(parent), selecting(false)
{
    setStyleSheet("QGraphicsView { border-style: none; }");
    setRenderHints(QPainter::Antialiasing);
    setSceneRect(-width()/2, -height()/2, width(), height());

    QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QAbstractScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

Canvas::Canvas(GraphScene* s, QWidget* parent)
    : Canvas(parent)
{
    QGraphicsView::setScene(s);
    scene = s;
}

void Canvas::customizeUI(Ui::MainWindow* ui)
{
    ui->menuView->deleteLater();
    ui->menuReference->deleteLater();

    connect(ui->actionCopy, &QAction::triggered,
            this, &Canvas::onCopy);
    connect(ui->actionCut, &QAction::triggered,
            this, &Canvas::onCut);
    connect(ui->actionPaste, &QAction::triggered,
            this, &Canvas::onPaste);
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);
    if (!event->isAccepted()) {
            if (event->button() == Qt::LeftButton) {
                click_pos = mapToScene(event->pos());
            }
            else if (event->button() == Qt::RightButton) {
                QMenu* m = new QMenu(this);

                Q_ASSERT(dynamic_cast<MainWindow*>(parent()));
                auto window = static_cast<MainWindow*>(parent());
                window->populateMenu(m, false);

                m->exec(QCursor::pos());
                m->deleteLater();
            }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);

    if (selecting)
    {
        QPainterPath p;
        p.addRect(QRectF(click_pos, drag_pos));
        scene->setSelectionArea(p);
        selecting = false;
        scene->invalidate(QRectF(), QGraphicsScene::ForegroundLayer);
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
    if (scene->mouseGrabberItem() == NULL && event->buttons() == Qt::LeftButton)
    {
        if (event->modifiers() & Qt::ShiftModifier)
        {
            drag_pos = mapToScene(event->pos());
            selecting = true;
            scene->invalidate(QRectF(), QGraphicsScene::ForegroundLayer);
        }
        else
        {
            auto d = click_pos - mapToScene(event->pos());
            setSceneRect(sceneRect().translated(d.x(), d.y()));
        }
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
    {
        return;
    }
    else if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
    {
        deleteSelected();
    }
    else if (event->key() == Qt::Key_A &&
                (event->modifiers() & Qt::ShiftModifier))
    {
        QMenu* m = new QMenu(this);

        Q_ASSERT(dynamic_cast<MainWindow*>(parent()));
        auto window = static_cast<MainWindow*>(parent());
        window->populateMenu(m, false);

        m->exec(QCursor::pos());
        m->deleteLater();
    }
}

void Canvas::drawBackground(QPainter* painter, const QRectF& rect)
{
    painter->setBrush(Colors::base00);
    painter->setPen(Qt::NoPen);
    painter->drawRect(rect);

    if (fabs(rect.left() - rect.right()) < 5e3 &&
        fabs(rect.top() - rect.bottom()) < 5e3)
    {
        const int d = 20;
        painter->setPen(Colors::base03);
        for (int i = int(rect.left() / d) * d; i < rect.right(); i += d)
            for (int j = int(rect.top() / d) * d; j < rect.bottom(); j += d)
                painter->drawPoint(i, j);
    }
}

void Canvas::drawForeground(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(rect);

    if (selecting)
    {
        painter->setPen(QPen(Colors::base05, 2));
        painter->drawRect(QRectF(click_pos, drag_pos));
    }
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

void Canvas::deleteSelected()
{
    QSet<Node*> nodes;
    QSet<QPair<const Datum*, Datum*>> links;

     // Find all selected links
    for (auto i : scene->selectedItems())
        if (auto c = dynamic_cast<Connection*>(i))
            links.insert(QPair<const Datum*, Datum*>(
                        c->getSource()->getDatum(),
                        c->getTarget()->getDatum()));
        else if (auto p = dynamic_cast<NodeInspector*>(i))
            nodes.insert(p->getNode());

    App::instance()->pushStack(new UndoDeleteMultiCommand(nodes, links));
}

void Canvas::makeNodeAtCursor(NodeConstructorFunction f)
{
    auto n = f(App::instance()->getGraph());
    App::instance()->pushStack(new UndoAddNodeCommand(n));

    auto inspector = getNodeInspector(n);
    Q_ASSERT(inspector);
    inspector->setSelected(true);
    inspector->setPos(mapToScene(mapFromGlobal(QCursor::pos())));
    inspector->setDragging(true);
    inspector->grabMouse();
}


void Canvas::onCopy()
{
    if (auto i = dynamic_cast<QGraphicsTextItem*>(scene->focusItem()))
    {
        QApplication::clipboard()->setText(i->textCursor().selectedText());
    }
    else
    {
        // Find all selected nodes
        QList<Node*> selected;
        for (auto i : scene->selectedItems())
            if (auto r = dynamic_cast<NodeInspector*>(i))
                selected << r->getNode();

        if (!selected.isEmpty())
        {
            QJsonArray out;
            const auto i = scene->inspectorPositions();
            for (auto n : selected)
                out << SceneSerializer::serializeNode(n, i);

            auto data = new QMimeData();
            data->setData("sb::canvas", QJsonDocument(out).toJson());
            QApplication::clipboard()->setMimeData(data);
        }
    }
}

void Canvas::onCut()
{
    if (auto i = dynamic_cast<QGraphicsTextItem*>(scene->focusItem()))
    {
        QApplication::clipboard()->setText(i->textCursor().selectedText());
        i->textCursor().insertText("");
    }
}

void Canvas::onPaste()
{
    if (auto i = dynamic_cast<QGraphicsTextItem*>(scene->focusItem()))
    {
        i->textCursor().insertText(QApplication::clipboard()->text());
    }
    else
    {
        auto data = QApplication::clipboard()->mimeData();
        if (data->hasFormat("sb::canvas"))
            pasteNodes(QJsonDocument::fromJson(
                        data->data("sb::canvas")).array());
    }
}

void Canvas::pasteNodes(QJsonArray array)
{
    QList<QPair<uint64_t, uint64_t>> uid_map;
    auto g = App::instance()->getGraph();

    {   // Get the next n UIDs and update the nodes in the array.
        std::list<uint64_t> uids = g->getUIDs(array.size());
        auto itr = uids.begin();
        for (int i=0; i < array.size(); ++i)
        {
            // Update this node's UID and store the change in uid_map
            auto node = array[i].toObject();
            uid_map << QPair<uint64_t, uint64_t>(node["uid"].toInt(), *itr);
            node["uid"] = int((*itr)++);
            array[i] = node;
        }

        // For every connection datum, remap UIDs in the expression
        for (int i=0; i < array.size(); ++i)
        {
            auto node = array[i].toObject();
            auto datums = node["datums"].toArray();
            for (int j=0; j < datums.size(); ++j)
            {
                auto d = datums[j].toObject();
                auto expr = d["expr"].toString();
                if (expr.startsWith(Datum::SIGIL_CONNECTION))
                    for (auto u : uid_map)
                        expr.replace(QString::number(u.first) + ".",
                                     QString::number(u.second) + ".");
                d["expr"] = expr;
                datums[j] = d;
            }
            node["datums"] = datums;
            array[i] = node;
        }
    }

    SceneDeserializer::Info ds;
    for (auto n_ : array)
    {
        auto n = n_.toObject();

        auto name = n["name"].toString();
        if (!g->isNameUnique(name.toStdString()))
        {
            // Trim trailing numbers from the node's name
            while (name.at(name.size() - 1).isNumber())
                name = name.left(name.size() - 1);
            if (name.isEmpty())
                name = "n";
            // Then use the remaining string as a prefix
            n["name"] = QString::fromStdString(g->nextName(name.toStdString()));
        }
        SceneDeserializer::deserializeNode(n, g, &ds);
    }

    // Update the inspector positions by shifting a bit down and over
    for (auto& i : ds.inspectors)
        i += QPointF(10, 10);

    // Pull out the nodes that were just appended to the graph
    QSet<Node*> nodes;
    {
        auto all_nodes = g->childNodes();
        auto itr = all_nodes.rbegin();
        for (int i=0; i < array.size(); ++i)
            nodes.insert(*(itr++));
    }

    // Select all of the nodes that were just pasted in
    scene->clearSelection();
    for (auto n : nodes)
        scene->getInspector(n)->setSelected(true);

    // Load inspector positions and apply them to the scene.
    scene->setInspectorPositions(ds.inspectors);

}

void Canvas::onJumpTo(Node* node)
{
    auto inspector = getNodeInspector(node);
    Q_ASSERT(inspector);

    auto a = new QPropertyAnimation(this, "CENTER");
    a->setDuration(100);
    a->setStartValue(getCenter());
    a->setEndValue(inspector->sceneBoundingRect().center());

    auto b = new QPropertyAnimation(this, "ZOOM");
    b->setDuration(100);
    b->setStartValue(getZoom());
    b->setEndValue(1);
    b->setEasingCurve(QEasingCurve::InQuart);

    a->start(QPropertyAnimation::DeleteWhenStopped);
    b->start(QPropertyAnimation::DeleteWhenStopped);
}

void Canvas::setCenter(QPointF p)
{
    auto t = sceneRect();
    setSceneRect(sceneRect().translated(p.x() - t.center().x(),
                                        p.y() - t.center().y()));
}

QPointF Canvas::getCenter() const
{
    auto t = sceneRect();
    return QPointF(t.center().x(), t.center().y());
}

void Canvas::setZoom(float z)
{
    auto t = transform();
    scale(z / t.m11(), z / t.m22());
}

float Canvas::getZoom() const
{
    return transform().m11();
}
