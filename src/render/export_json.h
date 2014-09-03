#ifndef EXPORT_JSON_H
#define EXPORT_JSON_H

#include <QObject>
#include <QTextStream>
#include <QMap>

#include "fab/types/shape.h"

class ExportJSONWorker : public QObject
{
    Q_OBJECT
public:
    explicit ExportJSONWorker(QMap<QString, Shape> s, QString filename);
public slots:
    void run();
signals:
    void finished();
protected:
    void writeBounds(Shape* shape, QTextStream* out);
    void writeColor(Shape* shape, QTextStream* out);

    QMap<QString, Shape> shapes;
    QString filename;
};

#endif
