#ifndef INSPECTOR_TITLE_H
#define INSPECTOR_TITLE_H

#include <QGraphicsObject>
#include <QGraphicsTextItem>

class NodeInspector;
class GraphicsButton;

class Node;

class InspectorTitle : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit InspectorTitle(Node* n, NodeInspector* parent);
    void setNameValid(bool v);

    QRectF boundingRect() const override;
    void setTitle(QString t);

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
     *  When the name field changes, call node->setName
     */
    void onNameChanged();

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
    Node* node;

    QGraphicsTextItem* name;
    QGraphicsTextItem* title;
    QList<GraphicsButton*> buttons;

    float padding;
};

#endif
