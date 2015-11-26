#include <Python.h>

#include <QDebug>

#include <QPainter>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>

#include "ui/main_window.h"

#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/inspector/inspector_title.h"
#include "ui/canvas/inspector/inspector_text.h"
#include "ui/canvas/inspector/inspector_row.h"
#include "ui/canvas/inspector/inspector_buttons.h"
#include "ui/canvas/inspector/inspector_export.h"
#include "ui/canvas/port.h"
#include "ui/canvas/connection.h"
#include "ui/canvas/graph_scene.h"

#include "ui/util/colors.h"

#include "graph/datum.h"
#include "graph/node.h"
#include "graph/graph.h"

#include "app/app.h"
#include "app/undo/undo_move.h"

///////////////////////////////////////////////////////////////////////////////

// 4 gets to edge of "buttons" in title-row, 4 more padding/margin
// Probably there is a 4 lurking in canvas/inspector/inspector_buttons
const int NodeInspector::TitleRightPadding = 8; 

NodeInspector::NodeInspector(Node* node)
    : node(node), title_row(NULL),
      export_button(new InspectorExportButton(this)),
      dragging(false), border(10), glow(false), show_hidden(false)
{
    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    // Redo layout when datums change
    node->installWatcher(this);

    // Construct the title row here (rather than in the colon initialization)
    // so that it gets datumsChanged events after the inspector (this prevents
    // a crash when certain buttons change their visibility before the
    // inspector has time to react).
    title_row = new InspectorTitle(node, this);
    // Bump the title row down a little bit.
    title_row->setPos(4, 2);

    // When the title row changes, redo layout as well.
    connect(title_row, &InspectorTitle::layoutChanged,
            this, &NodeInspector::onLayoutChanged);

    trigger(node->getState());
}

float NodeInspector::maxLabelWidth() const
{
    float label_width = 0;
    for (auto row : rows)
        label_width = fmax(label_width, row->label->boundingRect().width());
    return label_width;
}

QRectF NodeInspector::boundingRect() const
{
    float height = title_row->boundingRect().height() + 4;
    float width =  title_row->boundingRect().width() + TitleRightPadding;

    for (auto row=rows.begin(); row != rows.end(); ++row)
    {
        if (show_hidden || !row.value()->label->toPlainText().startsWith("_"))
        {
            height += row.value()->boundingRect().height() + 4;
            width = fmax(width, row.value()->boundingRect().width());
        }
    }
    if (export_button->isVisible())
        height += export_button->boundingRect().height() + 6;

    return QRectF(-border, -border, width + 2*border, height + 2*border);
}

void NodeInspector::onLayoutChanged()
{
    float min_width = title_row->minWidth();
    for (auto r : rows)
        min_width = fmax(min_width, r->minWidth());
    title_row->setWidth(min_width);
    title_row->updateLayout();

    // Add inspector rows in the order they appear.
    float y = 2 + title_row->boundingRect().height() + 4;
    for (Datum* d : node->childDatums())
    {
        if (rows.contains(d))
        {
            if (show_hidden ||
                !rows[d]->label->toPlainText().startsWith("_"))
            {
                rows[d]->show();
                rows[d]->setWidth(min_width);
                rows[d]->updateLayout();
                rows[d]->setPos(0, y);
                y += 4 + rows[d]->boundingRect().height();
            }
            else
            {
                rows[d]->hide();
            }
        }
    }
    auto w = boundingRect().width() / 2;
    export_button->setPos(w/2, y);
    export_button->setWidth(w);
    prepareGeometryChange();
}

void NodeInspector::trigger(const NodeState& state)
{
    QList<const Datum*> not_present = rows.keys();

    title_row->setNameValid(state.name_valid);

    for (Datum* d : state.datums)
    {
        if (d->getName().find("__") != 0 && !rows.contains(d))
        {
            rows[d] = new InspectorRow(d, this);
            connect(rows[d], &InspectorRow::layoutChanged,
                    this, &NodeInspector::onLayoutChanged);

            // If we had pending links, construct them now
            if (link_cache.contains(d))
            {
                scene()->addItem(new Connection(outputPort(d), link_cache[d]));
                link_cache.remove(d);
            }
        }
        not_present.removeAll(d);
    }

    for (auto d : not_present)
    {
        delete rows[d];
        rows.remove(d);
    }

    onLayoutChanged();
}

void NodeInspector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    const auto r = boundingRect().adjusted(border, border, -border, -border);

    if (glow)
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(QColor(255, 255, 255, Colors::base02.red()), 20));
        painter->drawRoundedRect(r, 8, 8);
    }

    painter->setBrush(Colors::base01);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(r, 8, 8);

    painter->setBrush(Colors::base03);
    QRectF br = title_row->boundingRect();
    br.setWidth(r.width());
    br.setHeight(br.height() + 2);
    painter->drawRoundedRect(br, 8, 8);
    br.setHeight(br.height()/2);
    br.moveTop(br.height());
    painter->drawRect(br);

    painter->setBrush(Qt::NoBrush);
    if (isSelected())
        painter->setPen(QPen(Colors::base05, 2));
    else
        painter->setPen(QPen(Colors::base03, 2));
    painter->drawRoundedRect(r, 8, 8);
}

Node* NodeInspector::getNode()
{
    return node;
}

OutputPort* NodeInspector::outputPort(const Datum* d) const
{
    for (auto row : rows)
        for (auto a : row->childItems())
        {
            OutputPort* p = dynamic_cast<OutputPort*>(a);
            if (p && p->getDatum() == d)
                return p;
        }
    return NULL;
}

InputPort* NodeInspector::inputPort(const Datum* d) const
{
    for (auto row : rows)
        for (auto a : row->childItems())
        {
            InputPort* p = dynamic_cast<InputPort*>(a);
            if (p && p->getDatum() == d)
                return p;
        }
    return NULL;
}

void NodeInspector::makeLink(const Datum* source, InputPort* target)
{
    if (rows.contains(source))
        scene()->addItem(new Connection(outputPort(source), target));
    else
        link_cache[source] = target;
}

void NodeInspector::focusNext(DatumTextItem* prev)
{
    bool next = false;
    InspectorRow* first = NULL;

    prev->clearFocus();

    for (Datum* d : node->childDatums())
    {
        if (rows.contains(d))
        {
            auto row = rows[d];
            if (prev == row->editor)
            {
                next = true;
            }
            else if (row->editor->isEnabled() && row->isVisible())
            {
                if (next)
                {
                    row->editor->setFocus();
                    return;
                }
                else if (!first)
                {
                    first = row;
                }
            }
        }
    }
    if (first)
        first->editor->setFocus();
}

void NodeInspector::focusPrev(DatumTextItem* next)
{
    InspectorRow* prev = NULL;

    next->clearFocus();

    for (Datum* d : node->childDatums())
    {
        if (rows.contains(d))
        {
            auto row = rows[d];
            if (next == row->editor && prev)
            {
                prev->editor->setFocus();
                return;
            }
            if (row->editor->isEnabled() && row->isVisible())
                prev = row;
        }
    }
    // Handle wrapping around (first-to-last)
    if (prev)
        prev->editor->setFocus();
}

void NodeInspector::setGlow(bool g)
{
    if (g != glow)
    {
        glow = g;
        prepareGeometryChange();
    }
}

void NodeInspector::setShowHidden(bool h)
{
    if (h != show_hidden)
    {
        show_hidden = h;
        onLayoutChanged();
        prepareGeometryChange();
    }
}

void NodeInspector::setTitle(QString title)
{
    title_row->setTitle(title);
}

void NodeInspector::setExportWorker(ExportWorker* worker)
{
    const bool had_worker = export_button->hasWorker();
    export_button->setWorker(worker);
    if (!had_worker)
        onLayoutChanged();
}

void NodeInspector::clearExportWorker()
{
    if (export_button->hasWorker())
    {
        export_button->clearWorker();
        onLayoutChanged();
    }
}

void NodeInspector::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (dragging)
    {
        setPos(event->scenePos());
        event->accept();
    }
    else
    {
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void NodeInspector::contextMenuEvent(QGraphicsSceneContextMenuEvent* e)
{
    Q_UNUSED(e);

    QString desc = QString::fromStdString(node->getName()); // + " (" + node->getTitle() + ")";

    auto menu = new QMenu();
    auto jump_to = new QAction("Show " + desc + " in viewport", menu);

    menu->addAction(jump_to);
    connect(jump_to, &QAction::triggered,
            [=](){
                emit static_cast<GraphScene*>(
                    scene())->jumpTo(node);
            });

    menu->exec(QCursor::pos());
    delete menu;
}

void NodeInspector::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);

    if (dragging)
    {
        ungrabMouse();
    }
    else if (event->button() == Qt::LeftButton)
    {
        // Store an Undo command for this drag
        auto delta = event->scenePos() -
                     event->buttonDownScenePos(Qt::LeftButton);
        static_cast<GraphScene*>(scene())->endDrag(delta);
    }
    dragging = false;
}

void NodeInspector::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverEnterEvent(event);
    emit(glowChanged(node, true));
}

void NodeInspector::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItem::hoverLeaveEvent(event);
    emit(glowChanged(node, false));
}
