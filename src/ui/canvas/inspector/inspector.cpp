#include <Python.h>

#include <QDebug>

#include <QPainter>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

#include "ui/main_window.h"

#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/inspector/inspector_text.h"
#include "ui/canvas/inspector/inspector_row.h"
#include "ui/canvas/inspector/inspector_menu.h"
#include "ui/canvas/port.h"
#include "ui/canvas/scene.h"

#include "ui/util/colors.h"

#include "graph/datum/datum.h"
#include "graph/datum/datums/script_datum.h"
#include "graph/node/node.h"

#include "app/app.h"
#include "app/undo/undo_move.h"

///////////////////////////////////////////////////////////////////////////////

NodeInspector::NodeInspector(Node* node)
    : node(node), name(NULL),
      title(new QGraphicsTextItem(node->getTitle(), this)),
      menu_button(new InspectorMenuButton(this)),
      dragging(false), border(10), glow(false)
{
    if (auto n = node->getDatum("_name"))
    {
        name = new DatumTextItem(n, this);
        name->setAsTitle();
        name->setPos(6, 2);
        connect(n, &Datum::changed,
                this, &NodeInspector::onLayoutChanged);
    }

    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);

    title->setPos(6, 2);
    title->setDefaultTextColor(Colors::base06);
    auto f = title->font();
    f.setBold(true);
    title->setFont(f);

    // Make connections for dynamic title changing
    // (which only happens for ScriptNodes)
    connect(node, &Node::titleChanged,
            title, &QGraphicsTextItem::setPlainText);
    connect(node, &Node::titleChanged,
            [=](QString){ this->onLayoutChanged(); });

    // Redo layout when datums change (also only for script nodes)
    connect(node, &Node::datumsChanged,
            this, &NodeInspector::onDatumsChanged);

    // Delete oneself when the target node is deleted
    connect(node, &Node::destroyed, this, &NodeInspector::deleteLater);

    populateLists(node);
}

float NodeInspector::labelWidth() const
{
    float label_width = 0;
    for (auto row : rows)
        label_width = fmax(label_width, row->label->boundingRect().width());

    // Special case if the name field is getting too long -- pad the label
    // width so that the name field fits without overlapping into the title
    // field (forgive the hard-coded magic numbers)
    if (name)
    {
        int free_space = label_width + 145 - title->boundingRect().width();
        if (name->boundingRect().width() > free_space)
            label_width += name->boundingRect().width() - free_space;
    }
    return label_width;
}

QRectF NodeInspector::boundingRect() const
{
    float height = title->boundingRect().height() + 4;
    float width =  title->boundingRect().width() +
                   name->boundingRect().width() +
                   menu_button->boundingRect().width() + 24;

    for (auto row : rows)
    {
        height += row->boundingRect().height() + 4;
        width = fmax(width, row->boundingRect().width());
    }
    return QRectF(-border, -border, width + 2*border, height + 2*border);
}

void NodeInspector::onLayoutChanged()
{
    // Right-align the title block
    if (name)
        title->setPos(
                boundingRect().right() - border - title->boundingRect().width()
                - menu_button->boundingRect().width() - 6, 2);

    // Position the menu to the far right of the title bar
    menu_button->setPos(boundingRect().right() - border -
                        menu_button->boundingRect().width() - 3, 5);

    if (node)
    {
        float y = 2 + title->boundingRect().height() + 4;
        for (Datum* d : node->findChildren<Datum*>())
        {
            if (rows.contains(d))
            {
                rows[d]->updateLayout();
                rows[d]->setPos(0, y);
                y += 4 + rows[d]->boundingRect().height();
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

void NodeInspector::populateLists(Node *node)
{
    QList<Datum*> not_present = rows.keys();

    for (Datum* d : node->findChildren<Datum*>(
                QString(), Qt::FindDirectChildrenOnly))
    {
        if (!d->objectName().startsWith("_") && !rows.contains(d))
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
    QRectF br = title->boundingRect();
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

ScriptDatum* NodeInspector::getScriptDatum() const
{
    return dynamic_cast<ScriptDatum*>(node->getDatum("_script"));
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
            else if (next && dynamic_cast<DatumTextItem*>(row->editor))
            {
                row->editor->setFocus();
                return;
            }
        }
    }
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
            if (next == row->editor)
            {
                if (prev)
                    prev->editor->setFocus();
                return;
            }
            prev = row;
        }
    }
}

void NodeInspector::setGlow(bool g)
{
    if (g != glow)
    {
        glow = g;
        prepareGeometryChange();
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

void NodeInspector::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);

    if (dragging)
    {
        ungrabMouse();
    }
    else
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
    Q_UNUSED(event);
    emit(glowChanged(node, true));
}

void NodeInspector::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    emit(glowChanged(node, false));
}
