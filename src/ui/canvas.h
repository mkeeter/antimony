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

protected:
    QGraphicsScene* scene;

    float scale;
    float pitch;
    float yaw;

};

#endif // CANVAS_H
