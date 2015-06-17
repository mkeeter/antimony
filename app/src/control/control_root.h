#ifndef CONTROL_ROOT_H
#define CONTROL_ROOT_H

#include <QObject>
#include <QMap>

#include "util/hash.h"

class Node;
class Control;
class Viewport;

class ControlRoot : public QObject
{
    Q_OBJECT
public:
    ControlRoot(Node* n);
    void registerControl(long index, Control* c);
    Control* get(long index) const;
    void makeProxiesFor(Viewport* v);

    void setGlow(bool g);

    void prune();

signals:
    void changeProxySelection(bool b);

protected:
    QMap<long, QPointer<Control>> controls;
    Node* node;

    bool selected;
};

#endif
