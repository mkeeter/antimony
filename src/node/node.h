#ifndef NODE_H
#define NODE_H

#include <QObject>

class Control;
struct NodeProxy;

class Node : public QObject
{
    Q_OBJECT
public:
    explicit Node(QObject* parent=0);
    NodeProxy* proxy();
    virtual QString prefix() const=0;

protected:
    Control* control;
};

#endif // NODE_H
