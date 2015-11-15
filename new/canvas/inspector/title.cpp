#include <Python.h>

#include <QFont>
#include <QTextDocument>

#include "canvas/inspector/title.h"
#include "canvas/inspector/frame.h"
#include "app/colors.h"

#include "graph/node.h"

int InspectorTitle::MIN_TITLE_PADDING = 20;
int InspectorTitle::BUTTON_PADDING = 4;

InspectorTitle::InspectorTitle(Node* n, InspectorFrame* parent)
    : QGraphicsObject(parent),
      name(new QGraphicsTextItem(QString::fromStdString(n->getName()), this)),
      title(new QGraphicsTextItem("", this)), title_padding(MIN_TITLE_PADDING)
{
    name->setTextInteractionFlags(Qt::TextEditorInteraction);

    connect(name->document(), &QTextDocument::contentsChanged,
            [=]() { n->setName(this->name->toPlainText().toStdString()); });

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

void InspectorTitle::setScriptValid(bool valid)
{

}

void InspectorTitle::setTitle(QString title)
{

}

////////////////////////////////////////////////////////////////////////////////

float InspectorTitle::minWidth() const
{
    float width = name->boundingRect().width() +
                  MIN_TITLE_PADDING +
                  title->boundingRect().width();

    // Take buttons into account here

    return width;
}

////////////////////////////////////////////////////////////////////////////////
