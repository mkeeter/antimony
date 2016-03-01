#pragma once

#include <QObject>

class ViewportView;

class DepthImage : public QObject
{
public:
    DepthImage(ViewportView* view);
};
