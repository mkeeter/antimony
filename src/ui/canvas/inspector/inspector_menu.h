#ifndef INSPECTOR_MENU_H
#define INSPECTOR_MENU_H

#include "ui/util/button.h"

class NodeInspector;

class InspectorMenuButton : public GraphicsButton
{
    Q_OBJECT
public:
    InspectorMenuButton(NodeInspector* parent);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget=0) override;
protected slots:
    void onPressed();
};

#endif
