#ifndef DATUM_H
#define DATUM_H

#include <Python.h>
#include <QObject>
#include <QSet>

class InputHandler;

class Datum : public QObject
{
    Q_OBJECT
public:
    explicit Datum(QString name, QObject* parent=0);
    virtual ~Datum();

signals:
    void changed();
    void disconnectFrom(Datum *me);
public slots:
    void update();
    void onDisconnectRequest(Datum* downstream);

protected:
    void connectUpstream(Datum* upstream);
    virtual PyObject* getValue() const=0;

    PyObject* value;
    bool valid;

    InputHandler* input_handler;

    friend class TestDatum;
};

#endif // DATUM_H
