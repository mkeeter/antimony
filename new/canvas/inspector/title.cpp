#include <Python.h>

#include <QFont>
#include <QTextDocument>

#include "canvas/inspector/title.h"
#include "canvas/inspector/frame.h"
#include "canvas/inspector/buttons.h"
#include "canvas/inspector/util.h"

#include "app/colors.h"

#include "graph/node.h"

int InspectorTitle::MIN_TITLE_PADDING = 20;
int InspectorTitle::BUTTON_PADDING = 4;

InspectorTitle::InspectorTitle(Node* n, InspectorFrame* parent)
    : QGraphicsObject(parent),
      name(new QGraphicsTextItem(QString::fromStdString(n->getName()), this)),
      title(new QGraphicsTextItem("omg", this)), title_padding(MIN_TITLE_PADDING)
{
    name->setTextInteractionFlags(Qt::TextEditorInteraction);

    connect(name->document(), &QTextDocument::contentsChanged,
            [=]() { n->setName(this->name->toPlainText().toStdString()); });
    connect(this, &InspectorTitle::layoutChanged,
            parent, &InspectorFrame::redoLayout);


    {   // Customize font and color of name and title fields
        auto f = name->font();
        f.setBold(true);

        for (auto t : {name, title})
        {
            t->setPos(0, 0);
            t->setDefaultTextColor(Colors::base06);
            t->setFont(f);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

QRectF InspectorTitle::boundingRect() const
{
    float width = title->pos().x()+
                  title->boundingRect().width();

    for (auto b : buttons)
            if (b->isVisible())
                width += BUTTON_PADDING + b->boundingRect().width();

    return QRectF(0, 0, width, name->boundingRect().height());
}

void InspectorTitle::setWidth(float width)
{
    title->setPos(name->boundingRect().width() + MIN_TITLE_PADDING +
                  width - minWidth(), 0);

    float x = title->pos().x() + title->boundingRect().width();
    float height = title->boundingRect().height();
    for (auto b : buttons)
        if (b->isVisible())
        {
            moveTo(b, QPointF(x, (height - b->boundingRect().height())/2));
            x += b->boundingRect().width() + 4;
        }

    prepareGeometryChange();
}

void InspectorTitle::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

////////////////////////////////////////////////////////////////////////////////

void InspectorTitle::setNameValid(bool valid)
{
    name->setDefaultTextColor(valid ? Colors::base06 : Colors::red);
}

void InspectorTitle::setTitle(QString new_title)
{
    title->setPlainText(new_title);
    prepareGeometryChange();
    emit(layoutChanged());
}

////////////////////////////////////////////////////////////////////////////////

float InspectorTitle::minWidth() const
{
    float width = name->boundingRect().width() +
                  MIN_TITLE_PADDING +
                  title->boundingRect().width();

    for (auto b : buttons)
            if (b->isVisible())
                width += BUTTON_PADDING + b->boundingRect().width();

    return width;
}

////////////////////////////////////////////////////////////////////////////////

void InspectorTitle::addButton(InspectorButton* b)
{
    buttons.push_back(b);
    emit(layoutChanged());
}

