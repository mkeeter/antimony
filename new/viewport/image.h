#pragma once

#include <QObject>
#include <QPainter>

class ViewportView;

class DepthImage : public QObject
{
public:
    DepthImage(ViewportView* view);

    /*
     *  Draws the given depth image
     *  (should be called inside a native painting block)
     */
    void draw(QPainter* painter);
};
