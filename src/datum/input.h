#ifndef INPUT_H
#define INPUT_H

#include <Python.h>
#include <QObject>
#include <QPointer>

class Connection;

class InputHandler : public QObject
{
    Q_OBJECT
public:
    explicit InputHandler(QObject* parent=0);
    virtual PyObject* getValue() const=0;
    virtual bool accepts(Connection* input) const=0;
    virtual void addInput(Connection* input)=0;
    virtual bool hasInput() const=0;
};

////////////////////////////////////////////////////////////////////////////////

class SingleInputHandler : public InputHandler
{
    Q_OBJECT
public:
    explicit SingleInputHandler(QObject* parent=0);
    virtual PyObject* getValue() const;
    virtual bool accepts(Connection* input) const;
    virtual void addInput(Connection* input);
    virtual bool hasInput() const;
protected:
    QPointer<Connection> in;
};

#endif // INPUT_H
