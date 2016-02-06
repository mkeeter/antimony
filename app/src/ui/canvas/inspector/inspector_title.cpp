#include <Python.h>

#include <QTextDocument>

#include "ui/canvas/inspector/inspector_title.h"
#include "ui/canvas/inspector/inspector_text.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/inspector/inspector_buttons.h"
#include "ui/util/button.h"

#include "graph/node.h"
#include "graph/script_node.h"
#include "graph/datum.h"

#include "ui/util/colors.h"

InspectorTitle::InspectorTitle(Node* n, NodeInspector* parent)
    : QGraphicsObject(parent), node(n),
      name(new QGraphicsTextItem(QString::fromStdString(n->getName()), this)),
      title(new QGraphicsTextItem("", this)),
      buttons({new InspectorShowHiddenButton(this, parent)}),
      padding(20)

{
    if (auto script_node = dynamic_cast<ScriptNode*>(n))
    {
        buttons.append(new InspectorScriptButton(script_node, this));
    }

    name->setTextInteractionFlags(Qt::TextEditorInteraction);

    auto f = name->font();
    f.setBold(true);

    for (auto t : {name, title})
    {
        t->setPos(0, 0);
        t->setDefaultTextColor(Colors::base06);
        t->setFont(f);
    }

    for (auto b : buttons)
        connect(b, &QGraphicsObject::visibleChanged,
                this, &InspectorTitle::onButtonsChanged);

    connect(name->document(), &QTextDocument::contentsChanged,
            this, &InspectorTitle::onNameChanged);

    // The layout needs to be redone once padding is set
    // (which is dependent on the parent NodeInspector)
}

void InspectorTitle::onNameChanged()
{
    node->setName(name->toPlainText().toStdString());
    if (updateLayout())
        emit(layoutChanged());
}

QRectF InspectorTitle::boundingRect() const
{
    const float height = name->boundingRect().height();
    float width = name->boundingRect().width() +
        padding +
        title->boundingRect().width() + 2;

    for (auto b : buttons)
        if (b->isVisible())
            width += b->boundingRect().width() + 4;
    width -= 4;
    return QRectF(0, 0, width, height);
}

void InspectorTitle::setNameValid(bool v)
{
    name->setDefaultTextColor(v ? Colors::base06 : Colors::red);
}

void InspectorTitle::setTitle(QString t)
{
    title->setPlainText(t);
    if (updateLayout())
        emit(layoutChanged());
}

float InspectorTitle::minWidth() const
{
    float width = name->boundingRect().width() + 20 // padding
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

    float x = name->boundingRect().width() + padding;
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
}

void InspectorTitle::onButtonsChanged()
{
    prepareGeometryChange();
    emit(layoutChanged());
}
