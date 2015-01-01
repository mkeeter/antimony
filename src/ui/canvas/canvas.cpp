#include <Python.h>

#include <QMouseEvent>
#include <QDebug>
#include <QMenu>
#include <QClipboard>
#include <QMimeData>

#include <cmath>

#include "ui/canvas/canvas.h"
#include "ui/canvas/scene.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/connection.h"
#include "ui/util/colors.h"
#include "ui/main_window.h"
#include "ui_main_window.h"

#include "graph/node/node.h"
#include "graph/node/root.h"
#include "graph/datum/datum.h"
#include "graph/node/serializer.h"
#include "graph/node/deserializer.h"
#include "graph/datum/link.h"

#include "app/app.h"
#include "app/undo/undo_add_node.h"
#include "app/undo/undo_delete_link.h"
#include "app/undo/undo_delete_node.h"

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

void Canvas::setupUI(Ui::MainWindow* ui)
{
    ui->menuView->deleteLater();

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
    if (!event->isAccepted() && event->button() == Qt::LeftButton)
        click_pos = mapToScene(event->pos());
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

        auto window = dynamic_cast<MainWindow*>(parent());
        Q_ASSERT(window);
        window->populateMenu(m, false);

        m->exec(QCursor::pos());
        m->deleteLater();
    }
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
    QSet<Link*> links;
    bool started = false;
    auto start = [&]{ App::instance()->beginUndoMacro("'delete'");
                      started = true; };

    // Find all selected links
    for (auto i : scene->selectedItems())
        if (auto c = dynamic_cast<Connection*>(i))
            links.insert(c->getLink());

    // Find all selected nodes (and any links that are attached to them)
    for (auto i : scene->selectedItems())
        if (auto p = dynamic_cast<NodeInspector*>(i))
        {
            auto n = p->getNode();
            nodes.insert(n);
            for (auto k : n->getLinks())
                links.insert(k);
        }


    // Push delete commands for each selected and connected link.
    for (auto k : links)
    {
        if (!started)
            start();
        App::instance()->pushStack(new UndoDeleteLinkCommand(k));
    }

    // Push delete commands for each selected node.
    for (auto n : nodes)
    {
        if (!started)
            start();
        App::instance()->pushStack(new UndoDeleteNodeCommand(n));
    }

    if (started)
        App::instance()->endUndoMacro();
}

void Canvas::makeNodeAtCursor(NodeConstructor f)
{
    auto n = f(0, 0, 0, 1, App::instance()->getNodeRoot());

    App::instance()->newNode(n);
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
            auto p = dynamic_cast<NodeRoot*>(selected[0]->parent());
            Q_ASSERT(p);
            NodeRoot temp_root;

            // Move the nodes to a temporary root for serialization
            for (auto n : selected)
                n->setParent(&temp_root);

            auto data = new QMimeData();
            data->setData("sb::canvas", SceneSerializer(
                        &temp_root, scene->inspectorPositions()).run());
            QApplication::clipboard()->setMimeData(data);

            for (auto n : selected)
                n->setParent(p);
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
        {
            NodeRoot temp_root;
            SceneDeserializer ds(&temp_root);
            ds.run(data->data("sb::canvas"));

            for (auto& i : ds.inspectors)
                i += QPointF(10, 10);

            scene->clearSelection();
            App::instance()->beginUndoMacro("'paste'");
            for (auto n : temp_root.findChildren<Node*>())
            {
                n->setParent(App::instance()->getNodeRoot());
                App::instance()->newNode(n);
                App::instance()->pushStack(new UndoAddNodeCommand(n, "'paste'"));
                scene->getInspector(n)->setSelected(true);
            }
            scene->setInspectorPositions(ds.inspectors);
            App::instance()->endUndoMacro();
        }
    }
}
