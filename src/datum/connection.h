#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>

class Datum;
class ConnectionControl;

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(Datum *parent);
protected:
    Datum* target;
    ConnectionControl* control;
};

#endif // CONNECTION_H
