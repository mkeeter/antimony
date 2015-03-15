#ifndef EXPORT_JSON_H
#define EXPORT_JSON_H

#include <Python.h>
#include <QObject>
#include <QJsonObject>
#include <QMap>

#include "fab/types/shape.h"

#define EXPORT_JSON_INFIX 1
#define EXPORT_JSON_PREFIX 0

class ExportJSONWorker : public QObject
{
    Q_OBJECT
public:
    explicit ExportJSONWorker(QMap<QString, Shape> s, QString filename,
                              bool format);
public slots:
    void run();
signals:
    void finished();
protected:
    QJsonObject writeBounds(Shape* shape);
    QJsonObject writeBody(Shape* shape);
    QJsonObject writeColor(Shape* shape);

    QMap<QString, Shape> shapes;
    QString filename;
    bool format;
};

#endif
