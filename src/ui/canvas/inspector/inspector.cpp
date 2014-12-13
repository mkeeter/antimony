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
#include "ui/canvas/port.h"

#include "ui/util/colors.h"

#include "graph/datum/datum.h"
#include "graph/datum/datums/script_datum.h"
#include "graph/node/node.h"

///////////////////////////////////////////////////////////////////////////////

NodeInspector::NodeInspector(Node* node)
    : node(node), title(new QGraphicsTextItem(node->getType(), this))
{
    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIsFocusable);

    title->setPos(6, 2);
    title->setDefaultTextColor(Colors::base06);
    auto f = title->font();
    f.setBold(true);
    title->setFont(f);

    if (node->getDatum<ScriptDatum>("script"))
    {
        connect(node->getDatum<ScriptDatum>("script"), SIGNAL(datumsChanged()),
                this, SLOT(onDatumsChanged()));
    }
    connect(node, &Node::destroyed, this, &NodeInspector::deleteLater);

    populateLists(node);
    setZValue(-2);
}

void NodeInspector::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
        node->deleteLater();
    else
        event->ignore();
}

float NodeInspector::labelWidth() const
{
    float label_width = 0;
    for (auto row : rows)
    {
        label_width = fmax(label_width, row->label->boundingRect().width());
    }
    return label_width;
}

QRectF NodeInspector::boundingRect() const
{
    float height = title->boundingRect().height() + 4;
    float width = title->boundingRect().width() + 12;

    for (auto row : rows)
    {
        height += row->boundingRect().height() + 4;
        width = fmax(width, row->boundingRect().width());
    }
    return QRectF(0, 0, width, height);
}

void NodeInspector::onLayoutChanged()
{
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
            connect(rows[d], SIGNAL(layoutChanged()),
                    this, SLOT(onLayoutChanged()));
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

    painter->setBrush(Colors::base01);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(boundingRect(), 8, 8);

    painter->setBrush(Colors::base03);
    QRectF br = title->boundingRect();
    br.setWidth(boundingRect().width());
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
    painter->drawRoundedRect(boundingRect(), 8, 8);
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

#if 0
void NodeInspector::openScript(Datum *d) const
{
    ScriptDatum* s = dynamic_cast<ScriptDatum*>(d);
    Q_ASSERT(s);
    App::instance()->getWindow()->openScript(s);
}
#endif

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
        ungrabMouse();
    dragging = false;
}
