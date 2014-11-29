#include <cmath>

#include <QGraphicsSceneMouseEvent>

#include "ui/viewport/viewport.h"
#include "ui/viewport/view_selector.h"
#include "ui/colors.h"

ViewSelectorButton::ViewSelectorButton(QString label, QPointF pos,
                                       QGraphicsItem* parent)
    : QGraphicsObject(parent), label(label)
{
    setAcceptHoverEvents(true);
    setPos(pos);
}

QRectF ViewSelectorButton::boundingRect() const
{
    return QRectF(0, 0, 25, 25);
}

void ViewSelectorButton::hoverEnterEvent(QGraphicsSceneHoverEvent* e)
{
    Q_UNUSED(e);
    emit(mouseEnter(label));
}

void ViewSelectorButton::hoverLeaveEvent(QGraphicsSceneHoverEvent* e)
{
    Q_UNUSED(e);
    emit(mouseLeave());
}

void ViewSelectorButton::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit(pressed());
    }
}

void ViewSelectorButton::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(QPen(Colors::base02, 1));
    painter->setBrush(QBrush(Colors::base01));
    painter->drawRect(boundingRect());
}

////////////////////////////////////////////////////////////////////////////////

ViewSelector::ViewSelector(Viewport* viewport)
    : QGraphicsTextItem()
{
    QVector<ViewSelectorButton*> buttons;

    float s = 25;
    QPointF o(-2*s - 5, -2*s - 5);
    buttons << new ViewSelectorButton("top", o + QPointF(s, 0), this)
            << new ViewSelectorButton("front", o + QPointF(s, s), this)
            << new ViewSelectorButton("left", o + QPointF(0, s), this)
            << new ViewSelectorButton("right", o + QPointF(2*s, s), this)
            << new ViewSelectorButton("back", o + QPointF(3*s, s), this)
            << new ViewSelectorButton("bottom", o + QPointF(s, 2*s), this);

    for (auto b : buttons)
    {
        connect(b, SIGNAL(mouseEnter(QString)),
                this, SLOT(setLabel(QString)));
        connect(b, SIGNAL(mouseLeave()),
                this, SLOT(clearLabel()));
    }

    connect(buttons[0], SIGNAL(pressed()), this, SLOT(onTopPressed()));
    connect(buttons[1], SIGNAL(pressed()), this, SLOT(onFrontPressed()));
    connect(buttons[2], SIGNAL(pressed()), this, SLOT(onLeftPressed()));
    connect(buttons[3], SIGNAL(pressed()), this, SLOT(onRightPressed()));
    connect(buttons[4], SIGNAL(pressed()), this, SLOT(onBackPressed()));
    connect(buttons[5], SIGNAL(pressed()), this, SLOT(onBottomPressed()));

    setDefaultTextColor(Colors::base04);

    viewport->scene->addItem(this);
    connect(this, SIGNAL(spinTo(float,float)),
            viewport, SLOT(spinTo(float,float)));
}

void ViewSelector::setLabel(QString label)
{
    setPlainText(label);
}

void ViewSelector::clearLabel()
{
    setPlainText("");
}

void ViewSelector::onTopPressed()
{
    emit(spinTo(0, 0));
}

void ViewSelector::onBottomPressed()
{
    emit(spinTo(0, -M_PI));
}

void ViewSelector::onLeftPressed()
{
    emit(spinTo(M_PI/2, -M_PI/2));
}

void ViewSelector::onRightPressed()
{
    emit(spinTo(-M_PI/2, -M_PI/2));
}

void ViewSelector::onBackPressed()
{
    emit(spinTo(-M_PI, -M_PI/2));
}

void ViewSelector::onFrontPressed()
{
    emit(spinTo(0, -M_PI/2));
}
