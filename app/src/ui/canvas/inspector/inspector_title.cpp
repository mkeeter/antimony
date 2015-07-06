#include <Python.h>

#include "ui/canvas/inspector/inspector_title.h"
#include "ui/canvas/inspector/inspector_text.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/inspector/inspector_buttons.h"
#include "ui/util/button.h"

#include "graph/node.h"
#include "graph/datum.h"

#include "ui/util/colors.h"

InspectorTitle::InspectorTitle(Node* n, NodeInspector* parent)
    : QGraphicsObject(parent),
      title(new QGraphicsTextItem("Title!", this)),
      buttons({new InspectorExportButton(this),
               new InspectorShowHiddenButton(this, parent),
               new InspectorScriptButton(n, this)}),
      padding(20)

{
    name = NULL;
    /*
    if (auto d = n->getDatum("__name"))
    {
        name = new DatumTextItem(d, this);
        name->setAsTitle();
        name->setPos(0, 0);
        connect(d, &Datum::changed,
                [=]() {
                if(this->updateLayout())
                    emit(layoutChanged()); });
    }
    */

    title->setPos(0, 0);
    title->setDefaultTextColor(Colors::base06);
    auto f = title->font();
    f.setBold(true);
    title->setFont(f);

    // Make connections for dynamic title changing
    n->installWatcher(this);

    for (auto b : buttons)
        connect(b, &QGraphicsObject::visibleChanged,
                this, &InspectorTitle::onButtonsChanged);

    // The layout needs to be redone once padding is set
    // (which is dependent on the parent NodeInspector)
}

void InspectorTitle::trigger(const NodeState& state)
{
    // Set the name here!
    if (this->updateLayout())
        emit layoutChanged();
}

QRectF InspectorTitle::boundingRect() const
{
    const float height = 10;//name->boundingRect().height();
    float width = 100 + //name->boundingRect().width() +
        padding +
        title->boundingRect().width() + 2;

    for (auto b : buttons)
        if (b->isVisible())
            width += b->boundingRect().width() + 4;
    width -= 4;
    return QRectF(0, 0, width, height);
}

float InspectorTitle::minWidth() const
{
    float width = 20 // name->boundingRect().width() + 20 // padding
        + title->boundingRect().width() // title
        + 2; // more padding

    for (auto b : buttons)
        if (b->isVisible())
            width += b->boundingRect().width() + 4;
    width -= 4;
    return width;
}

void InspectorTitle::setWidth(float width)
{
    padding = 20 + width - minWidth();
}

bool InspectorTitle::updateLayout()
{
    bool changed = false;
    const float h = boundingRect().height();

    //
    // Name stays put at 0,0
    //

    float x = 20; // name->boundingRect().width() + padding;
    QPointF tpos(x, 0);
    if (tpos != title->pos())
    {
        changed = true;
        title->setPos(tpos);
    }

    x += title->boundingRect().width() + 2;

    for (auto b : buttons)
        if (b->isVisible())
        {
            QPointF p(x, (h - b->boundingRect().height())/2);
            if (p != b->pos())
            {
                changed = true;
                b->setPos(p);
            }
            x += b->boundingRect().width() + 4;
        }

    return changed;
}

void InspectorTitle::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    // Nothing to do here
}

void InspectorTitle::onButtonsChanged()
{
    prepareGeometryChange();
    emit(layoutChanged());
}
