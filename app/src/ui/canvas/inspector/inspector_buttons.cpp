#include <Python.h>

#include <QPainter>
#include <QMenu>
#include <QAction>

#include "ui/canvas/inspector/inspector_buttons.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/inspector/inspector_title.h"

#include "graph/datum/datums/script_datum.h"
#include "ui/util/colors.h"
#include "export/export_worker.h"
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
    Q_ASSERT(!script.isNull());
    App::instance()->newEditorWindow(script);
}

////////////////////////////////////////////////////////////////////////////////

InspectorShowHiddenButton::InspectorShowHiddenButton(
        QGraphicsItem* parent, NodeInspector* inspector)
    : GraphicsButton(parent), toggled(false), inspector(inspector)
{
    connect(this, &GraphicsButton::pressed,
            this, &InspectorShowHiddenButton::onPressed);
    setToolTip("Show hidden datums");

    connect(inspector->getNode(), &Node::datumsChanged,
            this, &InspectorShowHiddenButton::onDatumsChanged);
    onDatumsChanged();
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

void InspectorShowHiddenButton::onDatumsChanged()
{
    auto node = inspector->getNode();

    for (auto d : node->findChildren<Datum*>(
                QString(), Qt::FindDirectChildrenOnly))
    {
        if (d->objectName().startsWith("_") &&
            !d->objectName().startsWith("__"))
        {
            if (!isVisible())
            {
                show();
                return;
            }
        }
    }

    if (isVisible())
        hide();
}

////////////////////////////////////////////////////////////////////////////////

InspectorExportButton::InspectorExportButton(QGraphicsItem* parent)
    : GraphicsButton(parent)
{
    connect(this, &GraphicsButton::pressed,
            this, &InspectorExportButton::onPressed);
    setToolTip("Export");
    hide();
}

QRectF InspectorExportButton::boundingRect() const
{
    return QRectF(0, 0, 10, 10);
}

void InspectorExportButton::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(QPen(hover ? Colors::base05 : Colors::base04, 2));
    painter->setBrush(Qt::NoBrush);
    painter->drawLine(0, 6, 10, 6);
    painter->drawLine(10, 6, 6, 10);
    painter->drawLine(10, 6, 6, 2);
}

void InspectorExportButton::clearWorker()
{
    if (worker)
        worker->deleteLater();
    worker.clear();

    if (isVisible())
        hide();
}

void InspectorExportButton::setWorker(ExportWorker* w)
{
    clearWorker();
    worker = w;
    if (!isVisible())
        show();
}

void InspectorExportButton::onPressed()
{
    if (worker)
        worker->run();
}
