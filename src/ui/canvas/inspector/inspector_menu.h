#ifndef INSPECTOR_MENU_H
#define INSPECTOR_MENU_H

#include <QPointer.h>

#include "ui/util/button.h"

class ScriptDatum;

class InspectorMenuButton : public GraphicsButton
{
    Q_OBJECT
public:
    InspectorMenuButton(ScriptDatum* s, QGraphicsItem* parent);
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget=0) override;
protected slots:
    void onPressed();
protected:
    QPointer<ScriptDatum> script;
};

#endif
