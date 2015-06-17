#ifndef LINK_H
#define LINK_H

#include <QObject>
#include <QPointer>

class Datum;

class Link : public QObject
{
    Q_OBJECT
public:
    explicit Link(Datum *parent);
    void setTarget(Datum* d);
    bool hasTarget() const { return !target.isNull(); }
    Datum* getTarget() const;
protected:
    QPointer<Datum> target;

    friend class Connection;
};

#endif // LINK_H
