#pragma once

#include <QGraphicsView>

class ViewportView : public QGraphicsView
{
public:
    ViewportView(QWidget* parent);

    /*
     *  Returns a generic matrix transform from the view
     */
    QMatrix4x4 getMatrix() const;
};
