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
#include "graph/datum/datums/shape_input_datum.h"
#include "graph/datum/datums/shape_function_datum.h"

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
        warning_message = "Non-script nodes were automatically upgraded for compatability.";
    }

    // We can load protocol versions 2 and higher
    // (where version 2 has automatic node upgrading)
    if (protocol_version >= 2)
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
    *in >> t;
    QString node_name;
    *in >> node_name;

    NodeType::NodeType node_type = static_cast<NodeType::NodeType>(t);

    // In protocol 3 or later, all nodes are script nodes
    if (protocol_version >= 3)
        Q_ASSERT(node_type == NodeType::SCRIPT);

    Node* node = new Node(NodeType::SCRIPT, p);
    node->setObjectName(node_name);

    // Deserialize inspector position
    QPointF i;
    *in >> i;
    inspectors[node] = i;

    quint32 datum_count;
    *in >> datum_count;
    for (unsigned d=0; d < datum_count; ++d)
        deserializeDatum(in, node);

    if (protocol_version == 2)
        upgradeNode(node, node_type);
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
        case DatumType::SHAPE_INPUT:
            datum = new ShapeInputDatum(name, node); break;
        case DatumType::SHAPE_OUTPUT:
            datum = new ShapeOutputDatum(name, node); break;
        case DatumType::SHAPE_FUNCTION:
            datum = new ShapeFunctionDatum(name, node); break;
    }

    if (auto e = dynamic_cast<EvalDatum*>(datum))
    {
        QString expr;
        *in >> expr;
        e->setExpr(expr);
    }
    else if (auto f = dynamic_cast<FunctionDatum*>(datum))
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

void SceneDeserializer::upgradeNode(Node* node, NodeType::NodeType type)
{
    if (type != NodeType::SCRIPT)
    {
        // Save the names of all of this node's datums, then remove them
        // (as they're going to be shuffled around when the script evaluates)
        QStringList datum_names;
        while (datums.last()->parent() == node &&
               datums.last()->objectName() != "_name")
        {
            datum_names.prepend(datums.last()->objectName());
            datums.removeLast();
        }

        // Make a script datum with the matching upgraded script
        new ScriptDatum("_script", getScript(type), node);

        // Then add back all of the datums (in the same order)
        // Any missing datums are replaced by NULL; better hope that
        // nothing is connected to them!
        for (auto d : datum_names)
            datums.append(node->findChild<Datum*>(d));
    }
}

QString SceneDeserializer::getScript(NodeType::NodeType type) const
{
    auto path = App::instance()->nodePath() + "/" + scriptPath(type);
    QFile file(path);
    file.open(QIODevice::ReadOnly);

    QTextStream in(&file);
    return in.readAll();
}

QString SceneDeserializer::scriptPath(NodeType::NodeType type) const
{
    switch (type) {
        case NodeType::CIRCLE:    return "2D/circle.node";
        case NodeType::TRIANGLE:  return "2D/triangle.node";
        case NodeType::POINT2D:   return "2D/point.node";
        case NodeType::RECTANGLE: return "2D/rectangle.node";
        case NodeType::TEXT:      return "2D/text.node";
        case NodeType::CUBE:      return "3D/cube.node";
        case NodeType::CYLINDER:  return "3D/cylinder.node";
        case NodeType::CONE:      return "3D/cone.node";
        case NodeType::EXTRUDE:   return "3D/extrude.node";
        case NodeType::SPHERE:    return "3D/sphere.node";
        case NodeType::POINT3D:   return "3D/point.node";
        case NodeType::UNION:     return "CSG/union.node";
        case NodeType::BLEND:     return "CSG/blend.node";
        case NodeType::INTERSECTION:  return "CSG/intersection.node";
        case NodeType::DIFFERENCE:    return "CSG/difference.node";
        case NodeType::OFFSET:    return "CSG/offset.node";
        case NodeType::CLEARANCE: return "CSG/clearance.node";
        case NodeType::SHELL:     return "CSG/shell.node";
        case NodeType::ATTRACT:   return "Deform/attract.node";
        case NodeType::REPEL:     return "Deform/repel.node";
        case NodeType::SCALEX:    return "Deform/scale_x.node";
        case NodeType::SCALEY:    return "Deform/scale_y.node";
        case NodeType::SCALEZ:    return "Deform/scale_z.node";
        case NodeType::ROTATEX:   return "Transform/rotate_x.node";
        case NodeType::ROTATEY:   return "Transform/rotate_y.node";
        case NodeType::ROTATEZ:   return "Transform/rotate_z.node";
        case NodeType::REFLECTX:  return "Transform/reflect_x.node";
        case NodeType::REFLECTY:  return "Transform/reflect_y.node";
        case NodeType::REFLECTZ:  return "Transform/reflect_z.node";
        case NodeType::RECENTER:  return "Transform/recenter.node";
        case NodeType::TRANSLATE: return "Transform/translate.node";
        case NodeType::ITERATE2D: return "Iterate/iterate_2d.node";
        case NodeType::ITERATE_POLAR: return "Iterate/iterate_polar.node";
        default:    Q_ASSERT(false);
    }
}
