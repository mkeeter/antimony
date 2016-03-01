#pragma once

#include <QObject>

#include "viewport/image.h"

class DatumProxy;
class ViewportView;

class RenderInstance : public QObject
{
    Q_OBJECT
public:
    RenderInstance(DatumProxy* parent, ViewportView* view);

protected:
    DepthImage image;
};
