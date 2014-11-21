#include <Python.h>

#include <QDataStream>

#include "graph/node/deserializer.h"
#include "graph/node/node.h"
#include "graph/node/manager.h"

#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/int_datum.h"
#include "graph/datum/datums/name_datum.h"
#include "graph/datum/datums/string_datum.h"
#include "graph/datum/datums/script_datum.h"
#include "graph/datum/types/output_datum.h"
#include "graph/datum/types/shape_datum.h"
#include "graph/datum/types/function_datum.h"

SceneDeserializer::SceneDeserializer(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here
}

void SceneDeserializer::run(QDataStream* in)
{
    QString sb;
    quint32 version_major;
    quint32 version_minor;
    *in >> sb >> version_major >> version_minor;

    deserializeNodes(in, NodeManager::manager());
    deserializeConnections(in);
}

void SceneDeserializer::deserializeNodes(QDataStream* in, QObject* p)
{
    quint32 count;
    *in >> count;
    for (unsigned i=0; i < count; ++i)
    {
        deserializeNode(in, p);
    }
}

void SceneDeserializer::deserializeNode(QDataStream* in, QObject* p)
{
    quint32 t;
    *in >> t;
    QString node_name;
    *in >> node_name;

    NodeType::NodeType node_type = static_cast<NodeType::NodeType>(t);

    Node* node = new Node(node_type, p);
    node->setObjectName(node_name);

    // Deserialize child nodes.
    deserializeNodes(in, node);

    quint32 datum_count;
    *in >> datum_count;
    for (unsigned d=0; d < datum_count; ++d)
    {
        deserializeDatum(in, node);
    }
}

void SceneDeserializer::deserializeDatum(QDataStream* in, Node* node)
{
    quint32 t;
    *in >> t;
    QString name;
    *in >> name;

    DatumType::DatumType datum_type = static_cast<DatumType::DatumType>(t);

    Datum* datum;

    switch (datum_type)
    {
        case DatumType::FLOAT:
            datum = new FloatDatum(name, node); break;
        case DatumType::INT:
            datum = new IntDatum(name, node); break;
        case DatumType::NAME:
            datum = new NameDatum(name, node); break;
        case DatumType::STRING:
            datum = new StringDatum(name, node); break;
        case DatumType::SCRIPT:
            datum = new ScriptDatum(name, node); break;
        case DatumType::SHAPE:
            datum = new ShapeDatum(name, node); break;
        case DatumType::SHAPE_OUTPUT:
            datum = new ShapeOutputDatum(name, node); break;
        case DatumType::SHAPE_FUNCTION:
            datum = new ShapeFunctionDatum(name, node); break;
    }

    EvalDatum* e = dynamic_cast<EvalDatum*>(datum);
    FunctionDatum* f = dynamic_cast<FunctionDatum*>(datum);
    if (e)
    {
        QString expr;
        *in >> expr;
        e->setExpr(expr);
    }
    else if (f)
    {
        QString function_name;
        QList<QString> function_args;
        *in >> function_name >> function_args;
        f->setFunction(function_name, function_args);
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
