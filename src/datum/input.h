#ifndef INPUT_H
#define INPUT_H

#include <Python.h>
#include <QObject>
#include <QPointer>

class Datum;

class InputHandler : public QObject
{
    Q_OBJECT
public:
    explicit InputHandler(QObject* parent=0);
    virtual PyObject* getValue() const=0;
    virtual bool accepts(Datum* input) const=0;
    virtual void addInput(Datum* input)=0;
};

////////////////////////////////////////////////////////////////////////////////

class SingleInputHandler : public InputHandler
{
    Q_OBJECT
public:
    explicit SingleInputHandler(QObject* parent=0);
    virtual PyObject* getValue() const;
    virtual bool accepts(Datum* input) const;
    virtual void addInput(Datum* input);
protected:
    QPointer<Datum> in;
};

#endif // INPUT_H
