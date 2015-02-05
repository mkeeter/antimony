#ifndef EXPORT_BITMAP_H
#define EXPORT_BITMAP_H

#include <QObject>

#include "fab/types/shape.h"

class ExportBitmapWorker : public QObject
{
    Q_OBJECT
public:
    explicit ExportBitmapWorker(Shape s, float resolution, float mm_per_unit,
                                QString filename);
public slots:
    void render();
signals:
    void finished();
protected:
    Shape shape;
    float resolution;
    float mm_per_unit;
    QString filename;
};

#endif
