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
    connect(name->document(), &QTextDocument::contentsChanged,
            this, &InspectorTitle::checkLayout);

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

    checkLayout();
}

////////////////////////////////////////////////////////////////////////////////

QRectF InspectorTitle::boundingRect() const
{
    float width = name->boundingRect().width() +
                  title_padding +
                  title->boundingRect().width();

    // Take buttons into account here

    return QRectF(0, 0, width, name->boundingRect().height());
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
    checkLayout();
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

bool InspectorTitle::updateLayout()
{
    bool changed = false;
    const float height = boundingRect().height();

    // Name stays put at 0,0

    float x = name->boundingRect().width() + title_padding;

    changed |= moveTo(title, QPointF(x, 0));
    x += title->boundingRect().width();

    for (auto b : buttons)
        if (b->isVisible())
        {
            changed |= moveTo(
                    b, QPointF(x, (height - b->boundingRect().height())/2));
            x += b->boundingRect().width() + 4;
        }

    return changed;
}

void InspectorTitle::checkLayout()
{
    if (updateLayout())
        emit(layoutChanged());
}

////////////////////////////////////////////////////////////////////////////////

void InspectorTitle::addButton(InspectorButton* b)
{
    buttons.push_back(b);
    checkLayout();
}

////////////////////////////////////////////////////////////////////////////////

