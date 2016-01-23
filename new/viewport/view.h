#pragma once

#include <QVector3D>
#include <QGraphicsView>

#include "viewport/gl.h"

class ViewportView : public QGraphicsView
{
public:
    ViewportView(QWidget* parent);

    /*
     *  Returns a generic matrix transform from the view
     */
    QMatrix4x4 getMatrix() const;

    /*
     *  Draw depth images in the background
     */
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    /*
     *  Draws foreground info (including axes)
     */
    void drawForeground(QPainter* painter, const QRectF& rect) override;

    /*  Publically accessible handle to get shaders and VBO  */
    ViewportGL gl;

protected:
    /*
     *  Draws X, Y, Z axes transformed with the viewport's matrix
     */
    void drawAxes(QPainter* painter) const;

    /*  Center of 3D scene  */
    QVector3D center;

    /*  Scale of 3D view (higher numbers are closer)  */
    float scale;

    /*  Angles for rotation  */
    float pitch;
    float yaw;
};
