#include <Python.h>

#include <QPainter>
#include <QMenu>
#include <QAction>

#include "ui/canvas/inspector/inspector_menu.h"
#include "ui/canvas/inspector/inspector.h"

#include "ui/util/colors.h"
#include "app/app.h"

InspectorMenuButton::InspectorMenuButton(NodeInspector* parent)
    : GraphicsButton(parent)
{
    connect(this, &GraphicsButton::pressed,
            this, &InspectorMenuButton::onPressed);
}

QRectF InspectorMenuButton::boundingRect() const
{
    return QRectF(0, 0, 20, 20);
}

void InspectorMenuButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(hover ? Colors::base05 : Colors::base04);
    painter->drawRect(2, 2, 16, 3);
    painter->drawRect(2, 8, 16, 3);
    painter->drawRect(2, 14, 16, 3);
}

void InspectorMenuButton::onPressed()
{
    NodeInspector* i = dynamic_cast<NodeInspector*>(parentObject());
    Q_ASSERT(i);

    ScriptDatum* s = i->getScriptDatum();
    Q_ASSERT(s);

    App::instance()->newEditorWindow(s);
}
