#ifndef CONTROL_ROOT_H
#define CONTROL_ROOT_H

#include <QObject>
#include <QMap>

class Node;
class Control;
class Viewport;

class ControlRoot : public QObject
{
public:
    ControlRoot(Node* n);
    void registerControl(long index, Control* c);
    Control* get(long index) const;
    void makeProxiesFor(Viewport* v);

    void setGlow(bool g);

    void prune();

protected:
    QMap<long, QPointer<Control>> controls;
    Node* node;
};

#endif
