#ifndef DATUM_H
#define DATUM_H

#include <Python.h>
#include <QObject>
#include <QList>
#include <QPointer>

#include "graph/datum/enum.h"

class InputHandler;
class Link;
class Node;
class NodeRoot;

class Datum : public QObject
{
    Q_OBJECT
public:
    explicit Datum(QString name, Node* parent);
    virtual ~Datum();

    /** Returns stored value. */
    PyObject* getValue() const { return value; }
    /** Returns stored valid flag. */
    bool      getValid() const { return valid; }

    /** Returns an enum for this datum's unique type
     *  (used in serialiation).
     */
    virtual DatumType::DatumType getDatumType() const=0;
    virtual QString getDatumTypeString() const=0;

    static Datum* fromTypeString(QString type, QString name, Node* parent);

    /** Returns desired Python type
     *
     *  Must be overloaded by derived classes
     */
    virtual PyTypeObject* getType() const=0;

    /** Returns True if there is an InputHandler.
     */
    bool hasInput() const { return input_handler != NULL; }

    /** Returns True if the InputHandler is provided an input value.
     */
    bool hasInputValue() const;

    /*
     *  Returns a list of all the input links
     */
    QList<Link*> inputLinks() const;

    /** Returns true if the datum should have an output port.
     */
    virtual bool hasOutput() const { return true; }

    /** Returns True if this datum is the parent of > 0 connected links.
     */
    bool hasConnectedLink() const;

    /** Returns True if this Datum can be edited in a viewer.
     */
    virtual bool canEdit() const;

    /** Returns a string suitable for display in a node viewer.
     */
    virtual QString getString() const=0;

    /** Creates a link that comes from this Datum with destination unknown.
     */
    Link* linkFrom();

    /** Returns True if this datum can accept the given link.
     */
    bool acceptsLink(Link* upstream);

    /** Adds the given link to this datum's input handler.
     */
    void addLink(Link* input);

    /** Connects the argument as an upstream Datum.
     *  Returns false if there's a recursive loop.
     */
    bool connectUpstream(Datum* upstream);

    /** Returns a list of datums that are inputs to the input handler.
     */
    QList<Datum*> getInputDatums() const;

    /*
     *  Looks up the root of this datum's branch of the hierarchy.
     */
    NodeRoot* root() const;

signals:
    /** Emitted when value, valid, or editable changes.
     */
    void changed();

    /** Emitted when connections change.
     */
    void connectionChanged();

    /** Emitted to request upstream datums to disconnect.
     */
    void disconnectFrom(Datum *me);

public slots:
    /** Called to update this datum.  May emit changed.
     */
    void update();

    /** Called to disconnect from a downstream datum.
     */
    void onDisconnectRequest(Datum* downstream);

protected:

    /** Returns true if the datum is already in an update() call
     *  When true, update should not be called again.
     *  (this is only really relevant for ScriptDatums).
     */
    virtual bool isRecursing() const { return false; }

    /** Find the actual value (through evaluation, lookup, function call)
     *
     *  Must be overloaded by child classes.
     */
    virtual PyObject* getCurrentValue()=0;

    /** Helper function that is called after construction
     *  (the first time that update is called)
     */
    virtual void postInit();

    PyObject* value;
    bool valid;

    InputHandler* input_handler;

    QList<QPointer<Datum>> _upstream;
    bool post_init_called;
};

#endif // DATUM_H
