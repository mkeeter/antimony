#include <Python.h>

#include <QFile>
#include <QDebug>
#include <QJsonValue>
#include <QJsonDocument>
#include <cmath>

#include "render/export_json.h"
#include "tree/node/printers_ss.h"

ExportJSONWorker::ExportJSONWorker(QMap<QString, Shape> s, QString filename,
                                   bool format)
    : QObject(), shapes(s), filename(filename), format(format)
{
    // Nothing to do here.
}

QJsonObject ExportJSONWorker::writeBounds(Shape* shape)
{
    QJsonObject bounds;
    bounds["xmin"] = shape->bounds.xmin;
    bounds["ymin"] = shape->bounds.ymin;
    bounds["zmin"] = isinf(shape->bounds.zmin) ? QJsonValue()
                                               : shape->bounds.zmin;
    bounds["xmax"] = shape->bounds.xmax;
    bounds["ymax"] = shape->bounds.ymax;
    bounds["zmax"] = isinf(shape->bounds.zmax) ? QJsonValue()
                                               : shape->bounds.zmax;
    return bounds;
}

QJsonObject ExportJSONWorker::writeColor(Shape* shape)
{
    QJsonObject color;
    color["R"] = "f" + QString::number(shape->r > 0 ? shape->r : 255);
    color["G"] = "f" + QString::number(shape->g > 0 ? shape->g : 255);
    color["B"] = "f" + QString::number(shape->b > 0 ? shape->b : 255);
    return color;
}

QJsonObject ExportJSONWorker::writeBody(Shape* shape)
{
    QJsonObject body;

    if (format == EXPORT_JSON_INFIX)
    {
        body["format"] = "infix";
        body["string"] = print_node_ss(shape->tree->head).c_str();
    }
    else if (format == EXPORT_JSON_PREFIX)
    {
        body["format"] = "prefix";
        body["string"] = shape->math.c_str();
    }
    return body;
}

void ExportJSONWorker::run()
{
    QJsonObject out;
    for (auto s=shapes.begin(); s != shapes.end(); ++s)
    {
        QJsonObject o;
        o["body"] = writeBody(&s.value());
        o["bounds"] = writeBounds(&s.value());
        o["color"] = writeColor(&s.value());
        out[s.key()] = o;
    }

    QFile output_file(filename);
    output_file.open(QFile::WriteOnly);
    output_file.write(QJsonDocument(out).toJson());

    emit(finished());
}
