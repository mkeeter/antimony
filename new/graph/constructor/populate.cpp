#include <Python.h>

#include <QObject>
#include <QDirIterator>
#include <QMenu>
#include <QTextStream>

#include "graph/constructor/populate.h"
#include "graph/constructor/constructor.h"
#include "app/app.h"

#include "graph/script_node.h"
#include "graph/graph_node.h"

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
                       callback(n);  });
}

static void populateFromFiles(QMenu* menu, Graph* g,
                              std::function<void(Node*)> callback)
{
    QDirIterator bitr(App::instance()->bundledNodePath(),
                      QDirIterator::Subdirectories);
    QDirIterator uitr(App::instance()->userNodePath(),
                      QDirIterator::Subdirectories);
    QList<QRegExp> title_regexs= {QRegExp(".*title\\('+([^']+)'+\\).*"),
                                  QRegExp(".*title\\(\"+([^\"]+)\"+\\).*")};

    // Extract all of valid filenames into a QStringList.
    QStringList node_filenames;
    for (auto itr : {&bitr, &uitr})
    {
        while (itr->hasNext())
        {
            auto n = itr->next();
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
void emptyCallback(Node* n)
{
    (void)n;
}

void populateNodeMenu(QMenu* menu, Graph* g,
                      std::function<void(Node*)> callback)
{
    // Hard-code important menu names to set their order.
    for (auto c : {"2D", "3D", "2D → 3D", "3D → 2D", "CSG"})
        menu->addMenu(c);
    menu->addSeparator();

    populateFromFiles(menu, g, callback);

    menu->addSeparator();

    addNodeToMenu(menu, QStringList(), "Script", g,
                [](Graph *r){ return new ScriptNode("s*",
                    "import fab\n\n"
                    "title('script')\n"
                    "input('r', float, 1)\n"
                    "output('c', fab.shapes.circle(0, 0, r))",
                    r); }, callback);
    addNodeToMenu(menu, QStringList(), "Graph", g,
            [](Graph *r){ return new GraphNode("g*", r); }, callback);
}
