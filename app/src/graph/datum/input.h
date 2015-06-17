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

    /** Return a string suitable for diplay in a node viewer.
     */
    virtual QString getString() const=0;

    /** Returns a list of all the datums connected to this input handler.
     * (calls getLinks internally, then looks up the parent of each one)
     */
    QList<Datum*> getInputDatums() const;

    /*
     *  Returns a list of links coming into this input handler.
     */
    virtual QList<Link*> getLinks() const=0;
};

////////////////////////////////////////////////////////////////////////////////

class SingleInputHandler : public InputHandler
{
public:
    SingleInputHandler(Datum* parent);
    PyObject* getValue() const override;
    bool accepts(Link* input) const override;
    void addInput(Link* input) override;
    bool hasInput() const override;
    QString getString() const override;
    QList<Link*> getLinks() const override;
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
    QString getString() const override;
    QList<Link*> getLinks() const override;
public slots:
    /** Removes inactive input pointers from the list.
     */
    void prune();
protected:
    /** Count the number of (live) inputs to this handler.
     */
    int inputCount() const;
    QList<QPointer<Link>> in;

};

#endif // INPUT_H
