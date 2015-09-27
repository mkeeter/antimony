#include <Python.h>

#include <QPainter>
#include <QMenu>
#include <QAction>

#include "ui/canvas/inspector/inspector_buttons.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/inspector/inspector_title.h"

#include "ui/util/colors.h"
#include "app/app.h"

#include "graph/script_node.h"

InspectorScriptButton::InspectorScriptButton(ScriptNode* n, QGraphicsItem* parent)
    : GraphicsButton(parent), node(n)
{
    setToolTip("Edit script");
    connect(this, &GraphicsButton::pressed,
            this, &InspectorScriptButton::onPressed);

    n->installScriptWatcher(this);
    trigger(n->getScriptState());
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
    const QColor base = script_valid ? Colors::base04 : Colors::red;
    painter->setBrush(hover ? Colors::highlight(base) : base);

    painter->drawRect(0, 0, 16, 3);
    painter->drawRect(0, 6, 16, 3);
    painter->drawRect(0, 12, 16, 3);
}

void InspectorScriptButton::trigger(const ScriptState& state)
{
    script_valid = (state.error_lineno == -1);
    prepareGeometryChange();
}

void InspectorScriptButton::onPressed()
{
    App::instance()->newEditorWindow(node);
}

////////////////////////////////////////////////////////////////////////////////

InspectorShowHiddenButton::InspectorShowHiddenButton(
        QGraphicsItem* parent, NodeInspector* inspector)
    : GraphicsButton(parent), toggled(false), inspector(inspector)
{
    connect(this, &GraphicsButton::pressed,
            this, &InspectorShowHiddenButton::onPressed);
    setToolTip("Show hidden datums");

    inspector->getNode()->installWatcher(this);
    trigger(inspector->getNode()->getState());
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

void InspectorShowHiddenButton::trigger(const NodeState& state)
{
    for (auto d : state.datums)
    {
        if (d->getName().find("_") == 0 &&
            d->getName().find("__") != 0)
        {
            if (!isVisible())
                show();
            return;
        }
    }

    if (isVisible())
        hide();
}

