#ifndef DATUM_H
#define DATUM_H

#include <Python.h>
#include <QObject>
#include <QList>
#include <QPointer>

class InputHandler;
class Link;

class Datum : public QObject
{
    Q_OBJECT
public:
    explicit Datum(QString name, QObject* parent=0);
    virtual ~Datum();

    /** Returns stored value. */
    PyObject* getValue() const { return value; }
    /** Returns stored valid flag. */
    bool      getValid() const { return valid; }

    /** Returns desired Python type
     *
     *  Must be overloaded by derived classes
     */
    virtual PyTypeObject* getType() const=0;

    /** Returns True if the InputHandler is provided an input value.
     */
    bool hasInputValue() const;

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
    bool acceptsLink(Link* upstream) const;

    /** Adds the given link to this datum's input handler.
     */
    void addLink(Link* input);

    /** Deletes an input link to the given datum.
     *  (or fails an assert if the datum is not linked to the input handler)
     */
    void deleteLink(Datum* upstream);

    /** Connects the argument as an upstream Datum.
     *  Returns false if there's a recursive loop.
     */
    bool connectUpstream(Datum* upstream);
signals:
    /** Emitted when value, valid, or editable changes.
     */
    void changed();

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
    /** Find the actual value (through evaluation, lookup, function call)
     *
     *  Must be overloaded by child classes.
     */
    virtual PyObject* getCurrentValue()=0;

    PyObject* value;
    bool valid;

    bool editable;
    QString repr;

    InputHandler* input_handler;

    QList<QPointer<Datum>> _upstream;
    bool _once;
};

#endif // DATUM_H
