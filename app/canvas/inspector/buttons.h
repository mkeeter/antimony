#pragma once

#include <QGraphicsObject>

class ScriptProxy;
class GraphProxy;

class InspectorTitle;
class InspectorFrame;

////////////////////////////////////////////////////////////////////////////////

class InspectorButton : public QGraphicsObject
{
    Q_OBJECT
public:
    InspectorButton(InspectorTitle* parent);

signals:
    void pressed();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

    bool hover;
};

////////////////////////////////////////////////////////////////////////////////

class InspectorScriptButton : public InspectorButton
{
    Q_OBJECT
public:
    InspectorScriptButton(ScriptProxy* n, InspectorTitle* parent);

    /*
     *  QGraphicsObject functions
     */
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget=0) override;

    /*
     *  Sets the color of the button and redraws.
     */
    void setScriptValid(bool valid);

protected:
    bool script_valid;
};

////////////////////////////////////////////////////////////////////////////////

class InspectorShowHiddenButton : public InspectorButton
{
    Q_OBJECT
public:
    InspectorShowHiddenButton(InspectorFrame* parent);

    /*
     *  QGraphicsItem functions
     */
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget=0) override;

protected:
    bool toggled;
    InspectorFrame* inspector;
};

////////////////////////////////////////////////////////////////////////////////

class InspectorGraphButton : public InspectorButton
{
    Q_OBJECT
public:
    InspectorGraphButton(GraphProxy* n, InspectorTitle* parent);

    /*
     *  QGraphicsItem overloaded functions
     */
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget=0) override;
};

class InspectorViewButton : public InspectorButton
{
    Q_OBJECT
public:
    InspectorViewButton(GraphProxy* n, InspectorTitle* parent);

    /*
     *  QGraphicsItem overloaded functions
     */
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget=0) override;
};

class InspectorQuadButton : public InspectorButton
{
    Q_OBJECT
public:
    InspectorQuadButton(GraphProxy* n, InspectorTitle* parent);

    /*
     *  QGraphicsItem overloaded functions
     */
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget=0) override;
};
