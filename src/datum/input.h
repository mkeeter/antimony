#ifndef INPUT_H
#define INPUT_H

#include <Python.h>
#include <QObject>
#include <QPointer>

class Link;
class Datum;

class InputHandler : public QObject
{
    Q_OBJECT
public:
    explicit InputHandler(Datum* parent);

    /** Gets a Python object representing the value from this input.
     *
     *  Returns NULL if the linked input is invalid.
     */
    virtual PyObject* getValue() const=0;

    /** Returns True if this input handler can accept the given link.
     */
    virtual bool accepts(Link* input) const=0;

    /** Adds the given link to this input handler.
     */
    virtual void addInput(Link* input)=0;

    /** Returns True if this input handler has a value to offer.
     */
    virtual bool hasInput() const=0;

    /** Delete the link to a particular upstream datum.
     */
    virtual void deleteInput(Datum* d)=0;

    /** Return a string suitable for diplay in a node viewer.
     */
    virtual QString getString() const=0;

    /** Returns a list of all the datums connected to this input handler.
     */
    virtual QList<Datum*> getInputDatums() const=0;
};

////////////////////////////////////////////////////////////////////////////////

class SingleInputHandler : public InputHandler
{
    Q_OBJECT
public:
    SingleInputHandler(Datum* parent);
    PyObject* getValue() const override;
    bool accepts(Link* input) const override;
    void addInput(Link* input) override;
    bool hasInput() const override;
    void deleteInput(Datum* d) override;
    QString getString() const override;
    QList<Datum*> getInputDatums() const override;
protected:
    QPointer<Link> in;
};

////////////////////////////////////////////////////////////////////////////////

class ShapeInputHandler : public InputHandler
{
    Q_OBJECT
public:
    ShapeInputHandler(Datum* parent);
    PyObject* getValue() const override;
    bool accepts(Link* input) const override;
    void addInput(Link* input) override;
    bool hasInput() const override;
    void deleteInput(Datum* d) override;
    QString getString() const override;
    QList<Datum*> getInputDatums() const override;
protected:
    /** Removes inactive input pointers from the list.
     */
    void prune();

    /** Count the number of (live) inputs to this handler.
     */
    int inputCount() const;
    QList<QPointer<Link>> in;

};

#endif // INPUT_H
