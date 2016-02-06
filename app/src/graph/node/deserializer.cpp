#include <Python.h>

#include <QDataStream>
#include <QJsonDocument>
#include <QBuffer>
#include <QFile>
#include <QTextStream>

#include "app/app.h"

#include "graph/node/deserializer.h"
#include "graph/node.h"
#include "graph/script_node.h"
#include "graph/graph.h"

PyObject* SceneDeserializer::globals = NULL;

bool SceneDeserializer::run(QJsonObject in, Graph* graph, Info* info)
{
    // Check that the "type" field is "sb"
    if (in.find("type") == in.end() || in["type"].toString() != "sb")
    {
        if (info)
            info->error_message = "Could not recognize file.<br><br>"
                                  "If this file was saved in Antimony 0.7.7 or earlier, "
                                  "open it in Antimony 0.7.8 and re-save to upgrade to the current file format.";
        return false;
    }

    // Check file saving protocol
    if (in.find("protocol") == in.end())
    {
        if (info)
            info->error_message = "Could not detect protocol";
        return false;
    }
    else
    {
        auto protocol_version = in["protocol"].toDouble();
#ifdef SUPPORT_PROTOCOL_5
        if (protocol_version < 5)
#else
        if (protocol_version < 6)
#endif
        {
            if (info)
                info->error_message = "File was saved with a older protocol and can no longer be read.";
            return false;
        }
#ifdef SUPPORT_PROTOCOL_5
        else if (protocol_version == 5)
        {
            updateGraph(&in);
        }
#endif
        else if (protocol_version > 6)
        {
            if (info)
                info->error_message = "File was saved with a newer protocol and cannot yet be read.";
            return false;
        }
    }

    // Make sure there's a "nodes" array
    if (in.find("nodes") == in.end() || !in["nodes"].isArray())
    {
        if (info)
            info->error_message = "File does not contain any nodes.";
        return false;
    }

    for (auto n : in["nodes"].toArray())
        deserializeNode(n.toObject(), graph, info);
    return true;
}

void SceneDeserializer::deserializeNode(QJsonObject in, Graph* p, Info* info)
{
    Node* node = NULL;
    if (in.contains("script"))
    {
        node = new ScriptNode(in["name"].toString().toStdString(),
                              in["uid"].toDouble(), p);
    }
    else
    {
        assert(false);
    }

    // Deserialize inspector position
    auto a = in["inspector"].toArray();
    if (info)
        info->inspectors[node] = QPointF(a[0].toDouble(), a[1].toDouble());

    for (auto d : in["datums"].toArray())
        deserializeDatum(d.toObject(), node);

    if (auto script_node = dynamic_cast<ScriptNode*>(node))
    {
        QStringList s;
        for (auto line : in["script"].toArray())
            s.append(line.toString());
        script_node->setScript(s.join("\n").toStdString());
    }
}

void SceneDeserializer::deserializeDatum(QJsonObject in, Node* node)
{
    if (!globals)
    {
        globals = Py_BuildValue("{sO}", "__builtins__", PyEval_GetBuiltins());
        PyDict_SetItemString(globals, "_fabtypes",
                             PyImport_ImportModule("_fabtypes"));
    }

    auto t = PyRun_String(
                  in["type"].toString().toStdString().c_str(),
                  Py_eval_input, globals, globals);
    Q_ASSERT(t);
    Q_ASSERT(PyType_Check(t));

    new Datum(in["name"].toString().toStdString(),
              in["uid"].toDouble(),
              in["expr"].toString().toStdString(),
              (PyTypeObject*)t, node);
}

#ifdef SUPPORT_PROTOCOL_5
void SceneDeserializer::updateGraph(QJsonObject* in)
{
    (*in)["protocol"] = 6;
    uint64_t uid = 0;
    QJsonArray nodes;
    for (auto n_ : (*in)["nodes"].toArray())
    {
        auto n = n_.toObject();
        n["uid"] = int(uid++);
        updateNode(&n);
        nodes.push_back(n);
    }

    auto conn = (*in)["connections"].toArray();
    for (auto c_ : conn)
    {
        auto c = c_.toArray();
        auto source = c[0].toArray();
        auto target = c[1].toArray();

        // Pick out the target source, then look for a matching Datum name
        // to get the source Datum's UID.
        uint64_t source_uid;
        bool found = false;
        {
            auto source_n = nodes[source[0].toInt()];
            for (auto d_ : source_n.toObject()["datums"].toArray())
            {
                if (d_.toObject()["name"].toString() == source[1].toString())
                {
                    source_uid = d_.toObject()["uid"].toInt();
                    found = true;
                }
            }
            Q_ASSERT(found);
        }

        // Then, modify the target node so that its expression
        // defines a connection.
        QJsonValueRef target_node_ref = nodes[target[0].toInt()];
        auto target_node = target_node_ref.toObject();
        QJsonValueRef target_datums_ref = target_node["datums"];
        QJsonArray datums_out;
        for (auto d_ : target_datums_ref.toArray())
        {
            auto d = d_.toObject();
            if (d["name"].toString() == target[1].toString())
            {
                auto expr = d["expr"].toString();
                if (expr.startsWith(Datum::SIGIL_CONNECTION))
                {
                    expr.chop(1);
                    expr += ",__" + QString::number(source[0].toInt()) +
                            ".__" + QString::number(source_uid) + "]";
                }
                else
                {
                    expr = Datum::SIGIL_CONNECTION + QString("[__") +
                           QString::number(source[0].toInt()) + ".__" +
                           QString::number(source_uid) + "]";
                }
                d["expr"] = expr;
            }
            datums_out.push_back(d);
        }
        target_datums_ref= datums_out;
        target_node_ref = target_node;
    }
    (*in)["nodes"] = nodes;
    in->erase(in->find("connections"));
}

void SceneDeserializer::updateNode(QJsonObject* in)
{
    QJsonArray datums;
    uint64_t uid = 0;
    for (auto d_ : (*in)["datums"].toArray())
    {
        auto d = d_.toObject();
        auto t = d["type"].toString();
        if (t == "script")
        {
            // Extract the script from the datum's expression
            QString s;
            for (auto line : d["expr"].toArray())
                s += line.toString() + "\n";

            // Update the script with new namespaces
            s.replace("fab.ui", "sb.ui");
            s.replace("fab.color", "sb.color");
            s.replace("meta.export_", "sb.export.");

            // If we use math functions, add an 'import math' call
            if (s.contains("math.") && !s.contains("import math"))
            {
                if (s.contains("\nimport"))
                    s.replace("\nimport", "\nimport math\nimport");
                else
                    s = "import math\n" + s;
            }

            // Put the script back into the file
            auto a = QJsonArray();
            for (auto line : s.split("\n"))
                a.append(line);
            (*in)["script"] = a;
        }
        else if (t == "name")
        {
            (*in)["name"] = d["expr"];
        }
        else
        {
            updateDatum(&d);
            d["uid"] = int(uid);
            datums.push_back(d);
        }
        uid++;
    }
    (*in)["datums"] = datums;
}

void SceneDeserializer::updateDatum(QJsonObject* in)
{
    auto t = (*in)["type"].toString();
    if (t == "float" || t == "float output")
    {
        (*in)["type"] = "float";
        if (!in->contains("expr"))
            (*in)["expr"] = "0.0";
    }
    else if (t == "shape" || t == "shape output")
    {
        (*in)["type"] = "_fabtypes.Shape";
        if (!in->contains("expr"))
            (*in)["expr"] = "None";
    }
    else if (t == "string" || t == "string output")
    {
        (*in)["type"] = "str";
        if (!in->contains("expr"))
            (*in)["expr"] = "''";
        else
            (*in)["expr"] = "'" + (*in)["expr"].toString() + "'";
    }
}

#endif
