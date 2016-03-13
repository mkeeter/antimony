#include <Python.h>

#include "graph/proxy/graph.h"
#include "graph/proxy/base_datum.h"
#include "graph/proxy/node.h"

#include "canvas/connection/connection.h"
#include "canvas/datum_port.h"
#include "canvas/scene.h"

#include "app/colors.h"

Connection::Connection(const Datum* source, BaseDatumProxy* target)
    : BaseConnection(Colors::getColor(source))
{
    setFlags(QGraphicsItem::ItemIsSelectable);

    // Get the GraphProxy in which our target datum lives
    const auto g = target->graphProxy();

    {   // Find the source port from the graph proxy
        auto p = g->getDatumProxy(const_cast<Datum*>(source));
        Q_ASSERT(p != NULL);

        source_port = p->outputPort();
    }
    target_port = target->inputPort();

    for (auto p : QList<DatumPort*>({source_port, target_port}))
    {
        connect(p, &DatumPort::moved,
                this, &Connection::onPortsMoved);
        connect(p, &DatumPort::hiddenChanged,
                this, &Connection::onHiddenChanged);
    }

    g->canvasScene()->addItem(this);
}

////////////////////////////////////////////////////////////////////////////////

QPointF Connection::startPos() const
{
    return source_port->mapToScene(source_port->portRect().center());
}

QPointF Connection::endPos() const
{
    return target_port->mapToScene(target_port->portRect().center());
}

////////////////////////////////////////////////////////////////////////////////

Datum* Connection::sourceDatum() const
{
    return source_port->getDatum();
}

Datum* Connection::targetDatum() const
{
    return target_port->getDatum();
}

////////////////////////////////////////////////////////////////////////////////

void Connection::onPortsMoved()
{
    prepareGeometryChange();
}

void Connection::onHiddenChanged()
{
    if (isHidden())
        hide();
    else
        show();
    prepareGeometryChange();
}

////////////////////////////////////////////////////////////////////////////////

bool Connection::isHidden() const
{
    return !source_port->isVisible() ||
           !target_port->isVisible();
}
