#include <Python.h>

#include <QObject>
#include <QDirIterator>
#include <QMenu>
#include <QTextStream>
#include <QInputDialog>

#include "graph/constructor/populate.h"
#include "graph/constructor/constructor.h"
#include "app/app.h"
#include "undo/undo_add_node.h"
#include "undo/undo_add_datum.h"

#include "graph/script_node.h"
#include "graph/graph_node.h"
#include "fab/fab.h"

static void addNodeToMenu(QMenu* menu, QStringList category, QString name,
                          Graph* g, NodeConstructorFunction f,
                          std::function<void(Node*)> callback)
{
    // Recurse down node directory structure
    while (!category.isEmpty())
    {
        bool found = false;
        for (auto m : menu->findChildren<QMenu*>(
                    QString(), Qt::FindDirectChildrenOnly))
        {
            if (m->title() == category.first())
            {
                menu = m;
                found = true;
                break;
            }
        }

        if (!found)
            menu = menu->addMenu(category.first());

        category.removeFirst();
    }
    menu->connect(menu->addAction(name), &QAction::triggered,
                  [=]{ Node* n = f(g);
                       App::instance()->pushUndoStack(
                           new UndoAddNode(n));
                       callback(n);  });
}

static void populateFromFiles(QMenu* menu, Graph* g,
                              std::function<void(Node*)> callback)
{
    QList<QRegExp> title_regexs= {QRegExp(".*title\\('+([^']+)'+\\).*"),
                                  QRegExp(".*title\\(\"+([^\"]+)\"+\\).*")};

    // Extract all of valid filenames into a QStringList.
    QStringList node_filenames;
    for (auto path : App::instance()->nodePaths())
    {
        QDirIterator itr(path, QDirIterator::Subdirectories);
        while (itr.hasNext())
        {
            auto n = itr.next();
            if (n.endsWith(".node"))
                node_filenames.append(n);
        }
    }

    // Sort the list, then populate menus.
    QMap<QString, QPair<QStringList, NodeConstructorFunction>> nodes;
    QStringList node_titles;
    for (auto n : node_filenames)
    {
        QFile file(n);
        if (!file.open(QIODevice::ReadOnly))
            continue;

        QTextStream in(&file);
        QString txt = in.readAll();

        // Find the menu structure for this node
        auto split = n.split('/');
        while (split.first() != "nodes")
            split.removeFirst();
        split.removeFirst();

        // Attempt to extract the title with a regex;
        // falling back to the node's filename otherwise.
        QString title = split.last().replace(".node","");
        split.removeLast();
        for (auto& regex : title_regexs)
            if (regex.exactMatch(txt))
                title = regex.capturedTexts()[1];

        QString name = "n*";
        if (title.size() > 0 && title.at(0).isLetter())
            name = title.at(0).toLower() + QString("*");
        NodeConstructorFunction constructor =
            [=](Graph *r){ return new ScriptNode(name.toStdString(),
                                                 txt.toStdString(), r); };
        nodes[title] = QPair<QStringList, NodeConstructorFunction>(
                split, constructor);
        node_titles.append(title);
    }

    // Put all of the nodes into the Add menu, deferring Export nodes
    // until the end (after a separator).
    node_titles.sort();
    QStringList deferred;
    for (auto title : node_titles)
        if (nodes[title].first.contains("Export"))
            deferred << title;
        else
            addNodeToMenu(menu, nodes[title].first, title,
                          g, nodes[title].second, callback);

    // Place the export nodes at the end of the list
    menu->addSeparator();
    for (auto title : deferred)
        addNodeToMenu(menu, nodes[title].first, title,
                      g, nodes[title].second, callback);
}

////////////////////////////////////////////////////////////////////////////////

Datum* makeDatum(GraphNode* node, PyTypeObject* type, bool output)
{
    bool ok;
    QString text = QInputDialog::getText(
            NULL, "Datum name?", "Datum name:",
            QLineEdit::Normal, "x", &ok);
    if (ok && !text.isEmpty())
        return node->makeDatum(text.toStdString(), type, output);
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////

void emptyNodeCallback(Node* n)
{
    (void)n;
}

void emptyDatumCallback(Datum* d)
{
    (void)d;
}

////////////////////////////////////////////////////////////////////////////////

void populateDatumCommands(QMenu* menu, GraphNode* node,
                          std::function<void(Datum*)> callback)
{
    auto inputs = menu->addMenu("Input");
    auto outputs = menu->addMenu("Output");

    QList<QPair<QString, PyTypeObject*>> items =
        {{"Float",  &PyFloat_Type},
         {"Int",    &PyLong_Type},
         {"String", &PyUnicode_Type},
         {"Shape", fab::ShapeType}};

    for (auto i : items)
    {
        inputs->connect(inputs->addAction(i.first), &QAction::triggered,
            [=](){  if (auto d = makeDatum(node, i.second, false))
                    {
                        App::instance()->pushUndoStack(
                           new UndoAddDatum(d));
                        callback(d);
                    }});
        outputs->connect(outputs->addAction(i.first), &QAction::triggered,
            [=](){  if (auto d = makeDatum(node, i.second, true))
                    {
                        App::instance()->pushUndoStack(
                           new UndoAddDatum(d));
                        callback(d);
                    }});
    }
}

void populateNodeMenu(QMenu* menu, Graph* g,
                      std::function<void(Node*)>  node_callback,
                      std::function<void(Datum*)> datum_callback)
{
    // Hard-code important menu names to set their order.
    for (auto c : {"2D", "3D", "2D → 3D", "3D → 2D", "CSG"})
        menu->addMenu(c);
    menu->addSeparator();

    populateFromFiles(menu, g, node_callback);

    menu->addSeparator();

    addNodeToMenu(menu, QStringList(), "Script", g,
                [](Graph *r){ return new ScriptNode("s*",
                    "import fab\n\n"
                    "title('script')\n"
                    "input('r', float, 1)\n"
                    "output('c', fab.shapes.circle(0, 0, r))",
                    r); }, node_callback);
    addNodeToMenu(menu, QStringList(), "Graph", g,
            [](Graph *r){ return new GraphNode("g*", r); }, node_callback);

    if (g->parentNode())
    {
        menu->addSeparator();
        populateDatumCommands(menu, g->parentNode(), datum_callback);
    }
}
