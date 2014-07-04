#ifndef CIRCLE_NODE_H
#define CIRCLE_NODE_H

#include <Python.h>

#include <QObject>
#include "node/node.h"

class CircleNode : public Node
{
    Q_OBJECT
public:
    explicit CircleNode(QString name, QString x, QString y, QString r,
                        QObject* parent=0);
    explicit CircleNode(float x, float y, float z, float scale,
                        QObject* parent=0);

    virtual Control* makeControl(Canvas *canvas) override;

    static QString menuName()     { return "Circle"; }
    static QString menuCategory() { return "2D"; }


};

#endif // CIRCLE_NODE_H
