#include <Python.h>

#include <QDebug>

#include "graph/datum/datum.h"
#include "graph/datum/input.h"
#include "graph/datum/link.h"

#include "graph/node/node.h"
#include "graph/node/root.h"

Datum::Datum(QString name, Node* parent)
    : QObject(parent), value(NULL), valid(false), input_handler(NULL),
      post_init_called(false)
{
    setObjectName(name);
}

Datum::~Datum()
{
    // Request that all upstream datums disconnect
    // (to prevent recursive loops in a partially-destroyed object)
    emit disconnectFrom(this);
    Py_XDECREF(value);
}

bool Datum::hasInputValue() const
{
    return input_handler != NULL && input_handler->hasInput();
}

QList<Link*> Datum::inputLinks() const
{
    return input_handler ? input_handler->getLinks()
                         : QList<Link*>();
}

bool Datum::canEdit() const
{
    return !hasInputValue();
}

bool Datum::acceptsLink(Link* upstream)
{
    if (dynamic_cast<Datum*>(upstream->parent())->_upstream.contains(this))
        return false;
    return input_handler->accepts(upstream);
}

Link* Datum::linkFrom()
{
    auto link = new Link(this);
    connect(link, &Link::destroyed, this, &Datum::connectionChanged);
    return link;
}

void Datum::addLink(Link* input)
{
    input_handler->addInput(input);
    input->setTarget(this);
    connect(this, &Datum::destroyed, input, &Link::deleteLater);
    connect(input, &Link::destroyed, this, &Datum::update);

    // For certain types of datums, making a connection changes behavior
    // in a way that requires a changed signal to be emitted.  This is
    // mostly relevant for shape datums, which are only rendered if they
    // are not used elsewhere in the system.
    emit(connectionChanged());
    emit(dynamic_cast<Datum*>(input->parent())->connectionChanged());
    connect(input, &Link::destroyed, this, &Datum::connectionChanged);
}

void Datum::deleteLink(Datum* upstream)
{
    input_handler->deleteInput(upstream);
}

bool Datum::hasConnectedLink() const
{
    for (auto link : findChildren<Link*>())
        if (link->hasTarget())
            return true;
    return false;
}

void Datum::update()
{
    // Prevent recursive calls and calls during destruction
    //
    // Recursive calls only happen for ScriptDatums, as new  datum creation
    // causes a name change update which tries to recursivey update the script
    // datum.
    //
    // Calls during deletion can happen anytime multiple Datums in the same
    // Node are connected; when the first Datum is deleted it will try to
    // update downstream values even as the Node is being deleted.
    if (isRecursing() || !dynamic_cast<Node*>(parent())
                      || !dynamic_cast<NodeRoot*>(parent()->parent()))
        return;

    // The very first time that update() is called, refresh all other nodes
    // that may refer to this node by name (then never do so again).
    if (!post_init_called)
        postInit();

    // Request that all upstream datums disconnect.
    emit disconnectFrom(this);
    _upstream.clear();
    _upstream << this;

    PyObject* new_value;
    if (hasInputValue())
        new_value = input_handler->getValue();
    else
        new_value = getCurrentValue();

    bool has_changed = false;
    // If our previous value was valid and our new value is invalid,
    // mark valid = false and emit a changed signal.
    if (new_value == NULL && valid)
    {
        valid = false;
        has_changed = true;
    }
    // If we've gone from invalid to valid or gotten a different object,
    // save the new value and emit changed.
    else if (new_value != NULL && (!valid ||
            PyObject_RichCompareBool(new_value, value, Py_NE)))
    {
        Py_XDECREF(value);
        value = new_value;
        Py_INCREF(value);

        valid = true;

        has_changed = true;
    }
    Py_XDECREF(new_value);

    // If our editable state has changed, mark that we need to emit the
    // changed signal (so that node viewers can modify their lineedits)
    if (canEdit() != editable)
    {
        editable = canEdit();
        has_changed = true;
    }

    // If the string representation has changed, then emit changed
    // so that node viewers can modify their text values.
    if (getString() != repr)
    {
        repr = getString();
        has_changed = true;
    }

    if (has_changed)
        emit changed();
}

void Datum::postInit()
{
    post_init_called = true;
    root()->onNameChange(objectName());
}

void Datum::onDisconnectRequest(Datum* downstream)
{
    disconnect(downstream, &Datum::disconnectFrom,
               this, &Datum::onDisconnectRequest);
    disconnect(this, &Datum::changed, downstream, &Datum::update);
    disconnect(this, &Datum::destroyed, downstream, &Datum::update);
}

bool Datum::connectUpstream(Datum* upstream)
{
    _upstream << upstream->_upstream;
    connect(upstream, &Datum::changed,
            this, &Datum::update, Qt::UniqueConnection);
    connect(upstream, &Datum::destroyed,
            this, &Datum::update, Qt::UniqueConnection);
    connect(this, &Datum::disconnectFrom,
            upstream, &Datum::onDisconnectRequest, Qt::UniqueConnection);
    return upstream->_upstream.contains(this) ? false : true;
}

QList<Datum*> Datum::getInputDatums() const
{
    return input_handler ? input_handler->getInputDatums()
                         : QList<Datum*>();
}

NodeRoot* Datum::root() const
{
    Q_ASSERT(parent() && dynamic_cast<Node*>(parent()));

    auto root = dynamic_cast<NodeRoot*>(parent()->parent());
    Q_ASSERT(root);

    return root;
}
