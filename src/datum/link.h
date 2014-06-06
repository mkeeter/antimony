#ifndef LINK_H
#define LINK_H

#include <QObject>

class Datum;
class LinkControl;

class Link : public QObject
{
    Q_OBJECT
public:
    explicit Link(Datum *parent);
protected:
    Datum* target;
    LinkControl* control;
};

#endif // LINK_H
