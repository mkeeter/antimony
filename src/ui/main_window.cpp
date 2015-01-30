#include <Python.h>

#include <QKeySequence>
#include <QMouseEvent>
#include <QDebug>
#include <QDirIterator>
#include <QRegExp>

#include "app/app.h"

#include "graph/node/node.h"
#include "graph/node/root.h"

#include "ui_main_window.h"
#include "ui/main_window.h"
#include "ui/canvas/canvas.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/viewport/viewport.h"
#include "ui/script/script_pane.h"

#include "control/proxy.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    connectActions(App::instance());
    ui->menuEdit->addAction(App::instance()->undoAction());
    ui->menuEdit->addAction(App::instance()->redoAction());
    setShortcuts();

    populateMenu(ui->menuAdd);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCentralWidget(QWidget* w)
{
    QMainWindow::setCentralWidget(w);

    if (auto c = dynamic_cast<Canvas*>(centralWidget()))
        c->customizeUI(ui);
    else if (auto e = dynamic_cast<ScriptPane*>(centralWidget()))
        e->customizeUI(ui);
    else
        for (auto v : findChildren<Viewport*>())
            v->customizeUI(ui);
}

void MainWindow::connectActions(App* app)
{
    // File menu
    connect(ui->actionSave, &QAction::triggered,
            app, &App::onSave);
    connect(ui->actionSaveAs, &QAction::triggered,
            app, &App::onSaveAs);
    connect(ui->actionNew, &QAction::triggered,
            app, &App::onNew);
    connect(ui->actionOpen, &QAction::triggered,
            app, &App::onOpen);
    connect(ui->actionQuit, &QAction::triggered,
            app, &App::quit);
    connect(ui->actionClose, &QAction::triggered,
            this, &MainWindow::deleteLater);

    // View window
    connect(ui->actionNewCanvas, &QAction::triggered,
            app, &App::newCanvasWindow);
    connect(ui->actionNewViewport, &QAction::triggered,
            app, &App::newViewportWindow);
    connect(ui->actionNewQuad, &QAction::triggered,
            app, &App::newQuadWindow);

    // Export menu
    connect(ui->actionExportMesh, &QAction::triggered,
            app, &App::onExportSTL);
    connect(ui->actionExportHeightmap, &QAction::triggered,
            app, &App::onExportHeightmap);
    connect(ui->actionExportJSON, &QAction::triggered,
            app, &App::onExportJSON);

    // Help menu
    connect(ui->actionAbout, &QAction::triggered,
            app, &App::onAbout);

    // Window title
    setWindowTitle(app->getWindowTitle());
    connect(app, &App::windowTitleChanged,
            this, &MainWindow::setWindowTitle);
}

void MainWindow::setShortcuts()
{
    ui->actionNew->setShortcuts(QKeySequence::New);
    ui->actionOpen->setShortcuts(QKeySequence::Open);
    ui->actionSave->setShortcuts(QKeySequence::Save);
    ui->actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    ui->actionClose->setShortcuts(QKeySequence::Close);
    ui->actionQuit->setShortcuts(QKeySequence::Quit);
    ui->actionCut->setShortcuts(QKeySequence::Cut);
    ui->actionCopy->setShortcuts(QKeySequence::Copy);
    ui->actionPaste->setShortcuts(QKeySequence::Paste);
}

bool MainWindow::isShaded() const
{
    return ui->actionShaded->isChecked();
}

////////////////////////////////////////////////////////////////////////////////

#include "graph/node/nodes/2d.h"
#include "graph/node/nodes/3d.h"
#include "graph/node/nodes/meta.h"
#include "graph/node/nodes/transforms.h"
#include "graph/node/nodes/deform.h"
#include "graph/node/nodes/iterate.h"

#include "graph/datum/types/eval_datum.h"

void MainWindow::createNew(bool recenter, NodeConstructorFunction f,
                           Viewport* v)
{
    v = v ? v : findChild<Viewport*>();
    auto c = findChild<Canvas*>();

    Q_ASSERT((v != NULL) ^ (c != NULL));

    QGraphicsView* view = (v != NULL) ?
        static_cast<QGraphicsView*>(v) :
        static_cast<QGraphicsView*>(c);

    if (recenter)
        QCursor::setPos(view->rect().center());

    if (v)
        v->makeNodeAtCursor(f);
    else if (c)
        c->makeNodeAtCursor(f);
}

void MainWindow::addNodeToMenu(QStringList category, QString name, QMenu* menu,
                               bool recenter, NodeConstructorFunction f,
                               Viewport* v)
{
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
    connect(menu->addAction(name), &QAction::triggered,
            [=]{ this->createNew(recenter, f, v); });
}

void MainWindow::populateBuiltIn(QMenu* menu, bool recenter, Viewport* v)
{
    auto add = [&](QString category, QString name, NodeConstructor constructor)
    {
        addNodeToMenu({category}, name, menu, recenter, constructor, v);
    };

    add("2D", "Circle", CircleNode);
    add("2D", "Point", Point2DNode);
    add("2D", "Triangle", TriangleNode);
    add("2D", "Rectangle", RectangleNode);
    add("2D", "Text", TextNode);

    add("3D", "Point", Point3DNode);
    add("3D", "Cube", CubeNode);
    add("3D", "Sphere", SphereNode);
    add("3D", "Cylinder", CylinderNode);
    add("3D", "Cone", ConeNode);
    add("3D", "Extrude", ExtrudeNode);

    add("Transform", "Rotate (X)", RotateXNode);
    add("Transform", "Rotate (Y)", RotateYNode);
    add("Transform", "Rotate (Z)", RotateZNode);
    add("Transform", "Reflect (X)", ReflectXNode);
    add("Transform", "Reflect (Y)", ReflectYNode);
    add("Transform", "Reflect (Z)", ReflectZNode);
    add("Transform", "Recenter", RecenterNode);
    add("Transform", "Translate", TranslateNode);

    add("Iterate", "Iterate (2D)", Iterate2DNode);
    add("Iterate", "Iterate (polar)", IteratePolarNode);

    add("Deform", "Attract", AttractNode);
    add("Deform", "Repel", RepelNode);
    add("Deform", "Scale (X)", ScaleXNode);
    add("Deform", "Scale (Y)", ScaleYNode);
    add("Deform", "Scale (Z)", ScaleZNode);
}

void MainWindow::populateUserScripts(QMenu* menu, bool recenter, Viewport* v)
{
    auto path = QCoreApplication::applicationDirPath().split("/");

#if defined Q_OS_MAC
    // On Mac, the 'nodes' folder should be either in
    // Antimony.app/Contents/Resources/nodes (when deployed)
    // or Antimony.app/../sb/nodes (when running from the build directory)
    path.removeLast(); // Trim the MacOS folder from the path

    // When deployed, the nodes folder is in Resources/sb
    if (QDir(path.join("/") + "/Resources/nodes").exists())
    {
        path.append("Resources");
    }
    // Otherwise, assume it's at the same level as antimony.app
    else
    {
        for (int i=0; i < 2; ++i)
            path.removeLast();
        path << "sb" << "nodes";
    }
#else
    path << "sb" << "nodes";
#endif

    QDirIterator itr(path.join("/"), QDirIterator::Subdirectories);
    QList<QRegExp> title_regexs= {QRegExp(".*title\\('+([^()']+)'+\\).*"),
                                  QRegExp(".*title\\(\"+([^\"()]+)\"+\\).*")};

    // Extract all of valid filenames into a QStringList.
    QStringList node_filenames;
    while (itr.hasNext())
    {
        auto n = itr.next();
        if (n.endsWith(".node"))
            node_filenames.append(n);
    }

    // Sort the list, then populate menus.
    node_filenames.sort();
    for (auto n : node_filenames)
    {
        auto split = n.split('/');
        while (split.first() != "nodes")
            split.removeFirst();
        split.removeFirst();

        QFile file(n);
        if (!file.open(QIODevice::ReadOnly))
            continue;

        QTextStream in(&file);
        QString txt = in.readAll();

        QString title = split.last().replace(".node","");
        split.removeLast();
        for (auto& regex : title_regexs)
            if (regex.exactMatch(txt))
                title = regex.capturedTexts()[1];

        NodeConstructorFunction constructor =
            [=](float x, float y, float z, float scale, NodeRoot *r)
            {
                auto s = ScriptNode(x, y, z, scale, r);
                static_cast<EvalDatum*>(s->getDatum("_script"))->setExpr(txt);
                return s;
            };
        addNodeToMenu(split, title, menu, recenter, constructor, v);
    }
}

void MainWindow::populateMenu(QMenu* menu, bool recenter, Viewport* v)
{
    for (auto c : {"2D", "3D", "CSG", "Transform", "Iterate", "Deform"})
        menu->addMenu(c);

    populateBuiltIn(menu, recenter, v);
    populateUserScripts(menu, recenter, v);

    menu->addSeparator();
    addNodeToMenu(QStringList(), "Script", menu, recenter,
                  static_cast<NodeConstructor>(ScriptNode), v);
}
