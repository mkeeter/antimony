#include <Python.h>

#include <QPainter>
#include <QMenu>
#include <QAction>

#include "ui/canvas/inspector/inspector_buttons.h"
#include "ui/canvas/inspector/inspector.h"

#include "graph/datum/datums/script_datum.h"
#include "ui/util/colors.h"
#include "app/app.h"

InspectorScriptButton::InspectorScriptButton(ScriptDatum* s, QGraphicsItem* parent)
    : GraphicsButton(parent), script(s)
{
    setToolTip("Edit script");
    connect(this, &GraphicsButton::pressed,
            this, &InspectorScriptButton::onPressed);
}

QRectF InspectorScriptButton::boundingRect() const
{
    return QRectF(0, 0, 16, 15);
}

void InspectorScriptButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(hover ? Colors::base05 : Colors::base04);
    painter->drawRect(0, 0, 16, 3);
    painter->drawRect(0, 6, 16, 3);
    painter->drawRect(0, 12, 16, 3);
}

void InspectorScriptButton::onPressed()
{
    Q_ASSERT(!s.isNull());
    App::instance()->newEditorWindow(script);
}

////////////////////////////////////////////////////////////////////////////////

InspectorShowHiddenButton::InspectorShowHiddenButton(
        QGraphicsItem* parent, NodeInspector* inspector)
    : GraphicsButton(parent), inspector(inspector)
{
    connect(this, &GraphicsButton::pressed,
            this, &InspectorShowHiddenButton::onPressed);
    setToolTip("Show hidden datums");
}

QRectF InspectorShowHiddenButton::boundingRect() const
{
    return QRectF(0, 0, 10, 15);
}

void InspectorShowHiddenButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(hover ? Colors::base06 :
                      toggled ? Colors::base04 : Colors::base02);
    painter->drawRect(0, 12, 10, 3);
    painter->drawEllipse(3, 4, 4, 4);
}

void InspectorShowHiddenButton::onPressed()
{
    toggled = !toggled;
    inspector->setShowHidden(toggled);
}
