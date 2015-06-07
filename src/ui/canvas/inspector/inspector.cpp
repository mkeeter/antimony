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
#include "ui/canvas/port.h"
#include "ui/canvas/graph_scene.h"

#include "ui/util/colors.h"

#include "graph/datum/datum.h"
#include "graph/datum/datums/script_datum.h"
#include "graph/node/node.h"

#include "app/app.h"
#include "app/undo/undo_move.h"

///////////////////////////////////////////////////////////////////////////////

NodeInspector::NodeInspector(Node* node)
    : node(node), title_row(NULL), dragging(false), border(10), glow(false),
      show_hidden(false)
{
    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);

    // Redo layout when datums change (also only for script nodes)
    connect(node, &Node::datumsChanged,
            this, &NodeInspector::onDatumsChanged);
    connect(node, &Node::datumOrderChanged,
            this, &NodeInspector::onDatumOrderChanged);

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

    // Delete oneself when the target node is deleted
    connect(node, &Node::destroyed, this, &NodeInspector::deleteLater);

    populateLists(node);
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
    // Special case if the node is being deleted
    if (node.isNull())
        return QRectF();

    float height = title_row->boundingRect().height() + 4;
    float width =  title_row->boundingRect().width() + 8;

    for (auto row=rows.begin(); row != rows.end(); ++row)
    {
        if (show_hidden || !row.key()->objectName().startsWith("_"))
        {
            height += row.value()->boundingRect().height() + 4;
            width = fmax(width, row.value()->boundingRect().width());
        }
    }
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
    if (node)
    {
        float y = 2 + title_row->boundingRect().height() + 4;
        for (Datum* d : node->findChildren<Datum*>())
        {
            if (rows.contains(d))
            {
                if (show_hidden || !d->objectName().startsWith("_"))
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
        prepareGeometryChange();
    }
}

void NodeInspector::onDatumsChanged()
{
    populateLists(node);
    onLayoutChanged();
}

void NodeInspector::onDatumOrderChanged()
{
    onLayoutChanged();
}

void NodeInspector::populateLists(Node *node)
{
    QList<Datum*> not_present = rows.keys();

    for (Datum* d : node->findChildren<Datum*>(
                QString(), Qt::FindDirectChildrenOnly))
    {
        if (!d->objectName().startsWith("__") && !rows.contains(d))
        {
            rows[d] = new InspectorRow(d, this);
            connect(rows[d], &InspectorRow::layoutChanged,
                    this, &NodeInspector::onLayoutChanged);
        }
        not_present.removeAll(d);
    }

    for (auto d : not_present)
    {
        rows[d]->deleteLater();
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

InputPort* NodeInspector::datumInputPort(Datum *d) const
{
    for (auto row : rows)
    {
        for (auto a : row->childItems())
        {
            InputPort* p = dynamic_cast<InputPort*>(a);
            if (p && p->getDatum() == d)
            {
                return p;
            }
        }
    }
    return NULL;
}

OutputPort* NodeInspector::datumOutputPort(Datum *d) const
{
    for (auto row : rows)
    {
        for (auto a : row->childItems())
        {
            OutputPort* p = dynamic_cast<OutputPort*>(a);
            if (p && p->getDatum() == d)
            {
                return p;
            }
        }
    }
    return NULL;
}

Node* NodeInspector::getNode()
{
    return node;
}

QPointF NodeInspector::datumOutputPosition(Datum* d) const
{
    OutputPort* p = datumOutputPort(d);
    Q_ASSERT(p);
    return p->mapToScene(p->boundingRect().center());
}

QPointF NodeInspector::datumInputPosition(Datum* d) const
{
    InputPort* p = datumInputPort(d);
    Q_ASSERT(p);
    return p->mapToScene(p->boundingRect().center());
}

void NodeInspector::focusNext(DatumTextItem* prev)
{
    bool next = false;
    InspectorRow* first = NULL;

    prev->clearFocus();

    for (Datum* d : node->findChildren<Datum*>())
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

    for (Datum* d : node->findChildren<Datum*>())
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
        emit(hiddenChanged());
    }
}

bool NodeInspector::isDatumHidden(Datum* d) const
{
    OutputPort* o = datumOutputPort(d);
    InputPort* i = datumInputPort(d);

    if (o)
        return !o->isVisible();
    if (i)
        return !i->isVisible();
    return false;
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

    QString desc = node->getName() + " (" + node->getTitle() + ")";

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

QVariant NodeInspector::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged)
        emit(moved());
    return value;
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
