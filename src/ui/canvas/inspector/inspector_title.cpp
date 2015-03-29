#include <Python.h>

#include "ui/canvas/inspector/inspector_title.h"
#include "ui/canvas/inspector/inspector_text.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/inspector/inspector_buttons.h"

#include "graph/node/node.h"
#include "graph/datum/datums/script_datum.h"

#include "ui/util/colors.h"

InspectorTitle::InspectorTitle(Node* n, NodeInspector* parent)
    : QGraphicsObject(parent),
      title(new QGraphicsTextItem(n->getTitle(), this)),
      script_button(new InspectorScriptButton(
                  n->getDatum<ScriptDatum>("__script"), this)),
      show_hidden_button(new InspectorShowHiddenButton(this, parent)),
      export_button(new InspectorExportButton(this)),
      padding(20)

{
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

    title->setPos(0, 0);
    title->setDefaultTextColor(Colors::base06);
    auto f = title->font();
    f.setBold(true);
    title->setFont(f);

    // Make connections for dynamic title changing
    connect(n, &Node::titleChanged,
            title, &QGraphicsTextItem::setPlainText);
    connect(n, &Node::titleChanged,
            [=](QString){
            if(this->updateLayout())
                emit(layoutChanged()); });

    // The layout needs to be redone once padding is set
    // (which is dependent on the parent NodeInspector)
}

QRectF InspectorTitle::boundingRect() const
{
    const float height = name->boundingRect().height();
    const float width = name->boundingRect().width() +
        padding +
        title->boundingRect().width()
        + 2
        + show_hidden_button->boundingRect().width()
        + 4
        + export_button->boundingRect().width()
        + 4
        + script_button->boundingRect().width();
    return QRectF(0, 0, width, height);
}

float InspectorTitle::minWidth() const
{
    return name->boundingRect().width() + 20 // padding
        + title->boundingRect().width() // title
        + 2 // more padding
        + show_hidden_button->boundingRect().width()
        + 4 // more padding
        + export_button->boundingRect().width()
        + 4 // more padding
        + script_button->boundingRect().width();
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

    float x = name->boundingRect().width() + padding;
    QPointF tpos(x, 0);
    if (tpos != title->pos())
    {
        changed = true;
        title->setPos(tpos);
    }

    x += title->boundingRect().width() + 2;
    QPointF hpos(x, (h - show_hidden_button->boundingRect().height())/2);
    if (hpos != show_hidden_button->pos())
    {
        changed = true;
        show_hidden_button->setPos(hpos);
    }

    x += show_hidden_button->boundingRect().width() + 4;
    QPointF epos(x, (h - export_button->boundingRect().height())/2);
    if (epos != export_button->pos())
    {
        changed = true;
        export_button->setPos(epos);
    }

    x += export_button->boundingRect().width() + 4;
    QPointF spos(x, (h - script_button->boundingRect().height()) / 2);
    if (spos != script_button->pos())
    {
        changed = true;
        script_button->setPos(spos);
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
