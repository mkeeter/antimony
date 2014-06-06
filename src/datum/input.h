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
    virtual PyObject* getValue() const=0;
    virtual bool accepts(Link* input) const=0;
    virtual void addInput(Link* input)=0;
    virtual bool hasInput() const=0;
};

////////////////////////////////////////////////////////////////////////////////

class SingleInputHandler : public InputHandler
{
    Q_OBJECT
public:
    SingleInputHandler(Datum* parent);
    virtual PyObject* getValue() const;
    virtual bool accepts(Link* input) const;
    virtual void addInput(Link* input);
    virtual bool hasInput() const;
protected:
    QPointer<Link> in;
};

#endif // INPUT_H
