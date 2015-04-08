#ifndef INSPECTOR_TITLE_H
#define INSPECTOR_TITLE_H

#include <QGraphicsObject>
#include <QGraphicsTextItem>

class Node;
class NodeInspector;
class DatumTextItem;
class ScriptDatum;
class GraphicsButton;

class InspectorTitle : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit InspectorTitle(Node* n, NodeInspector* parent);
    QRectF boundingRect() const;

    template <typename T> T* getButton() const
    {
        for (auto b : buttons)
            if (auto d = dynamic_cast<T*>(b))
                return d;
        return NULL;
    }

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

    /*
     *  When the buttons change, call prepareGeometryChange and emit
     *  layoutChanged.
     */
    void onButtonsChanged();
signals:
    void layoutChanged();

protected:
    DatumTextItem* name;
    QGraphicsTextItem* title;
    QList<GraphicsButton*> buttons;

    float padding;
};

#endif
