#ifndef INSPECTOR_BUTTONS_H
#define INSPECTOR_BUTTONS_H

#include <QPointer>

#include "ui/util/button.h"

#include "graph/watchers.h"

class NodeInspector;
class ExportWorker;

class Node;

////////////////////////////////////////////////////////////////////////////////

class InspectorScriptButton : public GraphicsButton
{
    Q_OBJECT
public:
    InspectorScriptButton(Node* n, QGraphicsItem* parent);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget=0) override;
    void setScriptValid(const bool v);
protected slots:
    void onPressed();
protected:
    bool script_valid;
    Node* node;
};

////////////////////////////////////////////////////////////////////////////////

class InspectorShowHiddenButton : public GraphicsButton, NodeWatcher
{
    Q_OBJECT
public:
    InspectorShowHiddenButton(QGraphicsItem* parent,
                              NodeInspector* inspector);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget=0) override;
    void trigger(const NodeState& state) override;
protected slots:
    void onPressed();
protected:
    bool toggled;
    NodeInspector* inspector;
};

#endif
