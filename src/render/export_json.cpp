#include <QFile>
#include <QDebug>
#include <cmath>

#include "render/export_json.h"

ExportJSONWorker::ExportJSONWorker(QMap<QString, Shape> s, QString filename)
    : QObject(), shapes(s), filename(filename)
{
    // Nothing to do here.
}

void ExportJSONWorker::writeBounds(Shape* shape, QTextStream* out)
{
    *out << "    \"bounds\": {\n";
    *out << "      \"xmin\": " << shape->bounds.xmin << ",\n";
    *out << "      \"ymin\": " << shape->bounds.ymin << ",\n";
    *out << "      \"zmin\": ";
    if (isinf(shape->bounds.zmin))
    {
        *out << "null,\n";
    }
    else
    {
        *out << shape->bounds.zmin << ",\n";
    }
    *out << "      \"xmax\": " << shape->bounds.xmax << ",\n";
    *out << "      \"ymax\": " << shape->bounds.ymax << ",\n";
    *out << "      \"zmax\": ";
    if (isinf(shape->bounds.zmax))
    {
        *out << "null\n";
    }
    else
    {
        *out << shape->bounds.zmin << "\n";
    }
    *out << "    },\n";
}

void ExportJSONWorker::writeColor(Shape* shape, QTextStream* out)
{
    Q_UNUSED(shape);
    // You can have any color you like, as long as it's white.
    *out << "    \"bounds\": {\n";
    *out << "      \"R\": \"f255\",\n";
    *out << "      \"G\": \"f255\",\n";
    *out << "      \"B\": \"f255\"\n";
    *out << "    }\n";
}

void ExportJSONWorker::run()
{
    QFile output_file(filename);
    output_file.open(QFile::WriteOnly);
    QTextStream out(&output_file);

    out << "{\n";
    for (auto s=shapes.begin(); s != shapes.end(); ++s)
    {
        out << "  \"" << s.key() << "\": {\n";
        out << "    \"body\": \"" << s.value().math.c_str() << "\",\n";
        writeBounds(&s.value(), &out);
        writeColor(&s.value(), &out);
        out << "  }";
        if (s + 1 != shapes.end())
        {
            out << ",";
        }
        out << "\n";
    }
    out << "}";

    emit(finished());
}
