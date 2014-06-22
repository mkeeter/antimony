#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMatrix4x4>

class Canvas : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Canvas(QWidget* parent=0);

    /** Returns our scale + rotation transform matrix.
     *  (translation is handled by the QGraphicsView)
     */
    QMatrix4x4 getMatrix() const;

    /** Transforms points from world to scene coordinates.
     */
    QPointF worldToScene(QVector3D v) const;
    QVector<QPointF> worldToScene(QVector<QVector3D> v) const;

    /** On mouse press, save mouse down position.
     */
    virtual void mousePressEvent(QMouseEvent *event) override;

    /** Transforms points from scene to world coordinates.
     */
    QVector3D sceneToWorld(QPointF p) const;

    QGraphicsScene* scene;

protected:
    /** Pans the scene rectangle.
     */
    void pan(QPointF d);

    float scale;
    float pitch;
    float yaw;

};

#endif // CANVAS_H
