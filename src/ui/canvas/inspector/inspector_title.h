#ifndef INSPECTOR_TITLE_H
#define INSPECTOR_TITLE_H

#include <QGraphicsObject>
#include <QGraphicsTextItem>

class Node;
class NodeInspector;
class DatumTextItem;
class InspectorScriptButton;
class ScriptDatum;

class InspectorTitle : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit InspectorTitle(Node* n, NodeInspector* parent);
    QRectF boundingRect() const;

    float minWidth() const;
    void setWidth(float width);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
public slots:
    /*
     *  Updates the title row's layout.
     *  Returns true if anything changed.
     */
    bool updateLayout();
signals:
    void layoutChanged();

protected:
    DatumTextItem* name;
    QGraphicsTextItem* title;
    InspectorScriptButton* script_button;

    float padding;
};

#endif
