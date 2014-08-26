#ifndef SCRIPT_BUTTONS_H
#define SCRIPT_BUTTONS_H

#include <QGraphicsObject>

class ScriptEditorButton : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit ScriptEditorButton(QGraphicsItem* parent);
protected:
    QRectF boundingRect() const override;
};

class ScriptEditorCloseButton : public ScriptEditorButton
{
public:
    explicit ScriptEditorCloseButton(QGraphicsItem* parent);
protected:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* o,
               QWidget* w) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* e);

    friend class ScriptEditorItem;
};

class ScriptEditorMoveButton : public ScriptEditorButton
{
public:
    explicit ScriptEditorMoveButton(QGraphicsItem* parent);
protected:
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* o,
               QWidget* w) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* e);
    void mouseMoveEvent(QGraphicsSceneMouseEvent* e);

    friend class ScriptEditorItem;
};

#endif
