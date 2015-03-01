#include <Python.h>

#include <QDataStream>
#include <QBuffer>
#include <QFile>
#include <QTextStream>

#include "app/app.h"

#include "graph/node/deserializer.h"
#include "graph/node/node.h"
#include "graph/node/root.h"

#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/float_output_datum.h"
#include "graph/datum/datums/int_datum.h"
#include "graph/datum/datums/name_datum.h"
#include "graph/datum/datums/string_datum.h"
#include "graph/datum/datums/script_datum.h"
#include "graph/datum/datums/shape_output_datum.h"
#include "graph/datum/datums/shape_datum.h"

SceneDeserializer::SceneDeserializer(NodeRoot* node_root)
    : QObject(), failed(false), node_root(node_root)
{
    // Nothing to do here
}

bool SceneDeserializer::run(QByteArray in)
{
    QBuffer buffer(&in);
    buffer.open(QBuffer::ReadOnly);

    QDataStream stream(&buffer);
    return run(&stream);
}

bool SceneDeserializer::run(QDataStream* in)
{
    QString sb;
    *in >> sb >> protocol_version;

    if (sb != "sb")
    {
        failed = true;
        error_message = "File is not an Antimony file";
    }
    else if (protocol_version < 2)
    {
        failed = true;
        error_message = "File was saved with an older protocol and can no longer be read.";
    }
    else if (protocol_version == 2)
    {
        failed = true;
        error_message =
            "File was saved with an older protocol and cannot be read.<br>"
            "Open it in Antimony 0.7.6c and re-save to upgrade file protocol.";
    }
    else if (protocol_version > 3)
    {
        failed = true;
        error_message = "File was saved with a newer protocol and cannot yet be read.";
    }

    if (!failed)
    {
        deserializeNodes(in, node_root);
        deserializeConnections(in);
    }

    return failed;
}

void SceneDeserializer::deserializeNodes(QDataStream* in, NodeRoot* p)
{
    quint32 count;
    *in >> count;
    for (unsigned i=0; i < count; ++i)
        deserializeNode(in, p);
}

void SceneDeserializer::deserializeNode(QDataStream* in, NodeRoot* p)
{
    quint32 t;
    *in >> t; // Deserialize dummy node type
    QString node_name;
    *in >> node_name;

    Node* node = new Node(p);
    node->setObjectName(node_name);

    // Deserialize inspector position
    QPointF i;
    *in >> i;
    inspectors[node] = i;

    quint32 datum_count;
    *in >> datum_count;
    for (unsigned d=0; d < datum_count; ++d)
        deserializeDatum(in, node);
}

void SceneDeserializer::deserializeDatum(QDataStream* in, Node* node)
{
    quint32 t;
    *in >> t;
    QString name;
    *in >> name;

    if (protocol_version == 3 && name == "_name")
        name = "__name";
    if (protocol_version == 3 && name == "_script")
        name = "__script";

    DatumType::DatumType datum_type = static_cast<DatumType::DatumType>(t);

    Datum* datum;

    switch (datum_type)
    {
        case DatumType::FLOAT:
            datum = new FloatDatum(name, node); break;
        case DatumType::FLOAT_OUTPUT:
            datum = new FloatOutputDatum(name, node); break;
        case DatumType::INT:
            datum = new IntDatum(name, node); break;
        case DatumType::NAME:
            datum = new NameDatum(name, node); break;
        case DatumType::STRING:
            datum = new StringDatum(name, node); break;
        case DatumType::SCRIPT:
            datum = new ScriptDatum(name, node); break;
        case DatumType::SHAPE_OUTPUT:
            datum = new ShapeOutputDatum(name, node); break;
        case DatumType::SHAPE_INPUT: // Automatically upgrade SHAPE_INPUT to SHAPE
        case DatumType::SHAPE:
            datum = new ShapeDatum(name, node); break;
        case DatumType::SHAPE_FUNCTION:
            datum = NULL;
            Q_ASSERT(false); // this is a deprecated Datum type.
    }

    auto e = dynamic_cast<EvalDatum*>(datum);
    // Special case when upgrading SHAPE_INPUT datums to SHAPE datums:
    // They weren't serialized with an expression, so don't try to read it.
    if (e && !(protocol_version == 3 && datum_type == DatumType::SHAPE_INPUT))
    {
        QString expr;
        *in >> expr;
        e->setExpr(expr);
    }

    datums << datum;
}

void SceneDeserializer::deserializeConnections(QDataStream* in)
{
    quint32 count;
    *in >> count;

    for (unsigned i=0; i < count; ++i)
    {
        quint32 source_index, target_index;
        *in >> source_index >> target_index;
        datums[target_index]->addLink(datums[source_index]->linkFrom());
    }
}
