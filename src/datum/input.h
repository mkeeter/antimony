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
};

////////////////////////////////////////////////////////////////////////////////

class SingleInputHandler : public InputHandler
{
    Q_OBJECT
public:
    SingleInputHandler(Datum* parent);
    virtual PyObject* getValue() const override;
    virtual bool accepts(Link* input) const override;
    virtual void addInput(Link* input) override;
    virtual bool hasInput() const override;
    virtual void deleteInput(Datum* d) override;
protected:
    QPointer<Link> in;
};

////////////////////////////////////////////////////////////////////////////////

class ShapeInputHandler : public InputHandler
{
    Q_OBJECT
public:
    ShapeInputHandler(Datum* parent);
    virtual PyObject* getValue() const override;
    virtual bool accepts(Link* input) const override;
    virtual void addInput(Link* input) override;
    virtual bool hasInput() const override;
    virtual void deleteInput(Datum* d) override;
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
