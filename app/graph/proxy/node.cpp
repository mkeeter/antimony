#include <Python.h>

#include "graph/proxy/node.h"
#include "graph/proxy/graph.h"
#include "graph/proxy/datum.h"
#include "graph/proxy/script.h"
#include "graph/proxy/util.h"

#include "canvas/inspector/frame.h"
#include "canvas/inspector/buttons.h"
#include "canvas/scene.h"

#include "viewport/control/control.h"

#include "graph/node.h"
#include "graph/graph_node.h"
#include "graph/script_node.h"

NodeProxy::NodeProxy(Node* n, GraphProxy* parent)
    : QObject(parent), node(n), script(NULL), subgraph(NULL),
      inspector(new InspectorFrame(n, parent->canvasScene())),
      show_hidden(new InspectorShowHiddenButton(inspector))
{
    if (auto graph_node = dynamic_cast<GraphNode*>(n))
    {
        subgraph = new GraphProxy(graph_node->getGraph(), this);
        connect(this, &NodeProxy::subnameChanged,
                subgraph, &GraphProxy::subnameChanged);
    }
    else if (auto script_node = dynamic_cast<ScriptNode*>(n))
    {
        script = new ScriptProxy(script_node->getScriptPointer(), this);
        connect(this, &NodeProxy::subnameChanged,
                script, &ScriptProxy::subnameChanged);
    }

    if (parent)
    {
        connect(parent, &GraphProxy::subnameChanged,
                this, &NodeProxy::onSubnameChanged);
    }

    connect(this, &QObject::destroyed, inspector, &QObject::deleteLater);

    connect(inspector, &InspectorFrame::onFocus, this, &NodeProxy::setFocus);
    connect(this, &NodeProxy::setFocus, inspector, &InspectorFrame::setFocus);

    connect(inspector, &InspectorFrame::onZoomTo, this, &NodeProxy::onZoomTo);

    n->installWatcher(this);
}

void NodeProxy::trigger(const NodeState& state)
{
    updateHash(state.datums, &datums, this);

    bool has_hidden = false;
    {   // Set indices of datums for proper sorting
        int i=0;
        for (auto d : state.datums)
        {
            datums[d]->setIndex(i++);
            has_hidden |= (d->getName().find("_") == 0 &&
                           d->getName().find("__") != 0);
        }
    }

    // Update inspector
    inspector->setNameValid(state.name_valid);

    if (has_hidden != show_hidden->isVisible())
    {
        show_hidden->setVisible(has_hidden);
    }

    {   // Delete any controls that weren't touched in the last script exec
        auto itr = controls.begin();
        while (itr != controls.end())
        {
            if (itr.value()->touched)
            {
                itr++;
            }
            else
            {
                itr.value()->deleteLater();
                itr = controls.erase(itr);
            }
        }
    }

    inspector->redoLayout();

    // Request that subgraph and script windows update their titles
    emit(subnameChanged(QString::fromStdString(node->getFullName())));
}

////////////////////////////////////////////////////////////////////////////////

CanvasInfo NodeProxy::canvasInfo() const
{
    CanvasInfo out;
    out.inspector[node] = inspector->pos();

    if (subgraph)
        out.unite(subgraph->canvasInfo());

    return out;
}

void NodeProxy::setPositions(const CanvasInfo& info, bool select)
{
    if (info.inspector.contains(node))
    {
        inspector->setPos(info.inspector[node]);
        if (select)
            inspector->setSelected(true);
    }

    if (subgraph)
        subgraph->setPositions(info, select);
}

////////////////////////////////////////////////////////////////////////////////

void NodeProxy::clearExportWorker()
{
    inspector->clearExportWorker();
}

void NodeProxy::setExportWorker(ExportWorker* worker)
{
    inspector->setExportWorker(worker);
}

////////////////////////////////////////////////////////////////////////////////

void NodeProxy::clearControlTouched()
{
    for (auto c : controls)
    {
        c->touched = false;
    }
}

////////////////////////////////////////////////////////////////////////////////

DatumProxy* NodeProxy::getDatumProxy(Datum* d)
{
    if (!datums.contains(const_cast<Datum*>(d)))
    {
        Q_ASSERT(d->parentNode() == node);
        datums[d] = new DatumProxy(d, this);
    }
    return datums[const_cast<Datum*>(d)];
}

////////////////////////////////////////////////////////////////////////////////

Control* NodeProxy::getControl(long lineno)
{
    return controls.contains(lineno) ? controls[lineno] : nullptr;
}

void NodeProxy::registerControl(long lineno, Control* c)
{
    Q_ASSERT(!controls.contains(lineno));
    controls[lineno] = c;
    static_cast<GraphProxy*>(parent())->makeInstancesFor(c);

    // Set highlight actions on focus
    connect(c, &Control::onFocus, this, &NodeProxy::setFocus);
    connect(this, &NodeProxy::setFocus, c, &Control::setFocus);

    // Call onZoomTo if control requests zooming
    connect(c, &Control::onZoomTo, this, &NodeProxy::onZoomTo);
}

////////////////////////////////////////////////////////////////////////////////

void NodeProxy::onSubnameChanged(QString ignored)
{
    Q_UNUSED(ignored);
    emit(subnameChanged(QString::fromStdString(node->getFullName())));
}

void NodeProxy::onZoomTo()
{
    static_cast<GraphProxy*>(parent())->zoomTo(node);
}

////////////////////////////////////////////////////////////////////////////////

void NodeProxy::makeInstancesFor(ViewportView* view)
{
    for (auto c : controls)
    {
        c->makeInstanceFor(view);
    }
    for (auto d : datums)
    {
        d->addViewport(view);
    }
}
