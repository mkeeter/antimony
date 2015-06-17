#include <Python.h>

#include <algorithm>

#include <QMouseEvent>
#include <QClipboard>
#include <QDebug>
#include <QPropertyAnimation>
#include <QSurfaceFormat>
#include <QMimeData>
#include <QMenu>
#include <QJsonDocument>

#include <QOpenGLWidget>

#include <cmath>

#include "ui/viewport/viewport.h"
#include "ui/viewport/depth_image.h"
#include "ui/viewport/view_selector.h"

#include "ui_main_window.h"
#include "ui/main_window.h"
#include "ui/util/colors.h"

#include "graph/datum/datum.h"
#include "graph/node/node.h"
#include "graph/node/serializer.h"
#include "graph/node/deserializer.h"
#include "graph/node/root.h"
#include "graph/datum/link.h"

#include "control/control.h"
#include "control/proxy.h"

#include "app/app.h"
#include "app/undo/undo_add_node.h"

Viewport::Viewport(QGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(parent), scene(scene),
      scale(100), pitch(0), yaw(0), angle_locked(false),
      view_selector(new ViewSelector(this)),
      gl_initialized(false), ui_hidden(false)
{
    setScene(scene);
    setStyleSheet("QGraphicsView { border-style: none; }");

    setSceneRect(-width()/2, -height()/2, width(), height());
    setRenderHints(QPainter::Antialiasing);

    auto gl = new QOpenGLWidget(this);
    setViewport(gl);
}

Viewport::~Viewport()
{
    for (auto d : findChildren<DepthImageItem*>())
        d->clearTextures();
}

void Viewport::customizeUI(Ui::MainWindow* ui)
{
    ui->menuReference->deleteLater();

    QActionGroup* view_actions = new QActionGroup(this);
    view_actions->addAction(ui->actionShaded);
    view_actions->addAction(ui->actionHeightmap);
    view_actions->setExclusive(true);

    // Accept the global command-line argument '--heightmap'
    // to always open scenes in height-map view.
    if (App::instance()->arguments().contains("--heightmap"))
        ui->actionHeightmap->setChecked(true);

    connect(ui->actionShaded, &QAction::triggered,
            [&]{ scene->invalidate(); });
    connect(ui->actionHeightmap, &QAction::triggered,
            [&]{ scene->invalidate(); });

    connect(ui->actionHideUI, &QAction::triggered,
            [&](bool b){ if (b) hideUI();
                         else   showUI(); });

    connect(ui->actionCopy, &QAction::triggered,
            this, &Viewport::onCopy);
    connect(ui->actionCut, &QAction::triggered,
            this, &Viewport::onCut);
    connect(ui->actionPaste, &QAction::triggered,
            this, &Viewport::onPaste);
}

void Viewport::initializeGL()
{
    float vbuf[] = {
         -1, -1,
         -1,  1,
          1, -1,
          1,  1};
    quad_vertices.create();
    quad_vertices.bind();
    quad_vertices.allocate(vbuf, sizeof(vbuf));
    quad_vertices.release();

    shaded_shader.addShaderFromSourceFile(
            QOpenGLShader::Vertex, ":/gl/quad.vert");
    shaded_shader.addShaderFromSourceFile(
            QOpenGLShader::Fragment, ":/gl/shaded.frag");
    shaded_shader.link();

    height_shader.addShaderFromSourceFile(
            QOpenGLShader::Vertex, ":/gl/quad.vert");
    height_shader.addShaderFromSourceFile(
            QOpenGLShader::Fragment, ":/gl/height.frag");
    height_shader.link();

    gl_initialized = true;
}

void Viewport::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    view_selector->setPos(width()/2 - 70, -height()/2 + 70);
    setSceneRect(-width()/2, -height()/2, width(), height());
}

QMatrix4x4 Viewport::getMatrix() const
{
    QMatrix4x4 M;

    // Remember that these operations are applied in reverse order.
    M.scale(scale, -scale, scale);
    M.rotate(pitch * 180 / M_PI, QVector3D(1, 0, 0));
    M.rotate(yaw  *  180 / M_PI, QVector3D(0, 0, 1));
    M.translate(-center.x(), -center.y(), -center.z());

    return M;
}

QMatrix4x4 Viewport::getTransformMatrix() const
{
    QMatrix4x4 M;

    // Remember that these operations are applied in reverse order.
    M.rotate(pitch * 180 / M_PI, QVector3D(1, 0, 0));
    M.rotate(yaw  *  180 / M_PI, QVector3D(0, 0, 1));

    return M;
}


QPointF Viewport::worldToScene(QVector3D v) const
{
    QMatrix4x4 M = getMatrix();
    QVector3D w = M * v;
    return QPointF(w.x(), w.y());
}

QVector<QPointF> Viewport::worldToScene(QVector<QVector3D> v) const
{
    QVector<QPointF> out;
    for (auto p : v)
        out << worldToScene(p);
    return out;
}

QVector3D Viewport::sceneToWorld(QPointF p) const
{
    QMatrix4x4 M = getMatrix().inverted();
    return M * QVector3D(p.x(), p.y(), 0);
}

void Viewport::makeNodeAtCursor(NodeConstructorFunction f)
{
    auto n = f(App::instance()->getNodeRoot());
    App::instance()->newNode(n);
    App::instance()->pushStack(new UndoAddNodeCommand(n));
}

float Viewport::getZmax() const
{
    float zmax = -INFINITY;
    for (auto p : findChildren<DepthImageItem*>())
        zmax = fmax((getTransformMatrix() * p->pos).z() + p->size.z()/2,
                    zmax);
    return zmax;
}

float Viewport::getZmin() const
{
    float zmin = INFINITY;
    for (auto p : findChildren<DepthImageItem*>())
        zmin = fmin((getTransformMatrix() * p->pos).z() - p->size.z()/2,
                    zmin);
    return zmin;
}

void Viewport::lockAngle(float y, float p)
{
    yaw = y;
    pitch = p;
    angle_locked = true;

    update();
    scene->invalidate(QRect(), QGraphicsScene::ForegroundLayer);
    emit(viewChanged());
}

QPair<char, float> Viewport::getAxis() const
{
    const auto M = getTransformMatrix();
    const float threshold = 0.98;

    const auto a = M.inverted() * QVector3D(0, 0, 1);

    QList<QPair<char, QVector3D>> axes = {
        {'x', QVector3D(1, 0, 0)},
        {'y', QVector3D(0, 1, 0)},
        {'z', QVector3D(0, 0, 1)}};

    for (const auto v : axes)
    {
        float dot = fabs(QVector3D::dotProduct(a, v.second));
        if (dot > threshold)
            return QPair<char, float>(v.first, (dot - threshold) / (1 - threshold));
    }
    return QPair<char, float>('\0', 0);
}

void Viewport::hideViewSelector()
{
    view_selector->hide();
}

QList<ControlProxy*> Viewport::getControlProxies(Node* n)
{
    QList<ControlProxy*> out;
    for (auto i : items())
    {
        auto p = dynamic_cast<ControlProxy*>(i);
        if (p && p->getNode() == n && !p->getControl()->parent())
            out << p;
    }
    return out;
}

QList<ControlProxy*> Viewport::getProxiesAtPosition(QPoint pos) const
{
    QSet<Node*> used;
    QList<ControlProxy*> out;

    for (auto i : items(pos))
    {
        auto c = dynamic_cast<ControlProxy*>(i);
        if (c)
        {
            auto n = c->getNode();
            if (!used.contains(n))
            {
                used << n;
                out << c;
             }
        }
    }
    return out;
}

QOpenGLBuffer* Viewport::getQuadVertices()
{
    if (!gl_initialized)
        initializeGL();
    return &quad_vertices;
}

QOpenGLShaderProgram* Viewport::getShadedShader()
{
    if (!gl_initialized)
        initializeGL();
    return &shaded_shader;
}

QOpenGLShaderProgram* Viewport::getHeightmapShader()
{
    if (!gl_initialized)
        initializeGL();
    return &height_shader;
}

void Viewport::spinTo(float new_yaw, float new_pitch)
{
    QPropertyAnimation* a = new QPropertyAnimation(this, "_yaw", this);
    a->setDuration(100);
    a->setStartValue(yaw);
    a->setEndValue(new_yaw);

    QPropertyAnimation* b = new QPropertyAnimation(this, "_pitch", this);
    b->setDuration(100);
    b->setStartValue(pitch);
    b->setEndValue(new_pitch);

    a->start(QPropertyAnimation::DeleteWhenStopped);
    b->start(QPropertyAnimation::DeleteWhenStopped);
}

void Viewport::mousePressEvent(QMouseEvent *event)
{
    _dragging = false;

    QGraphicsView::mousePressEvent(event);

    // If the event hasn't been accepted, record click position for
    // panning / rotation on mouse drag.
    if (!event->isAccepted())
    {
        if (event->button() == Qt::LeftButton)
        {
            _click_pos = mapToScene(event->pos());
            _click_pos_world = sceneToWorld(_click_pos);
        }
        else
        {
            _click_pos = event->pos();
        }
    }
}

void Viewport::mouseMoveEvent(QMouseEvent *event)
{
    _dragging = true;
    _current_pos = event->pos();

    QGraphicsView::mouseMoveEvent(event);
    if (scene->mouseGrabberItem() == NULL)
    {
        if (event->buttons() == Qt::LeftButton)
        {
            pan(_click_pos_world - sceneToWorld(mapToScene(event->pos())));
        }
        else if (event->buttons() == Qt::RightButton && !angle_locked)
        {
            QPointF d = _click_pos - event->pos();
            pitch = fmin(0, fmax(-M_PI, pitch - 0.01 * d.y()));
            yaw = fmod(yaw + M_PI - 0.01 * d.x(), M_PI*2) - M_PI;

            _click_pos = event->pos();
            update();
            scene->invalidate(QRect(),QGraphicsScene::ForegroundLayer);
            emit(viewChanged());
        }
    }

    scene->invalidate(QRect(), QGraphicsScene::ForegroundLayer);
}

void Viewport::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);

    // On right-click, show a menu of items to raise.
    if (event->button() == Qt::RightButton && !_dragging)
    {
        // Find the top-level MainWindow and attach the menu to it
        QObject* w = this;
        while (!dynamic_cast<MainWindow*>(w))
            w = w->parent();
        Q_ASSERT(w);

        // Make a new menu object
        auto menu = new QMenu(static_cast<MainWindow*>(w));

        // Find all of the nodes and proxies at the mouse click position
        auto proxies = getProxiesAtPosition(event->pos());

        // Special menu item to trigger a jump to event in the graph
        QAction* jump_to = NULL;

        for (auto p : proxies)
        {
            auto n = p->getNode();
            QString desc = n->getName() + " (" + n->getTitle() + ")";

            if (jump_to == NULL)
            {
                jump_to = new QAction("Show " + desc + " in graph", menu);
                menu->addAction(jump_to);
                menu->addSeparator();
                connect(jump_to, &QAction::triggered,
                        [=](){ emit jumpTo(n); });
            }

            auto a = new QAction(desc, menu);
            a->setData(QVariant::fromValue(p));
            menu->addAction(a);
        }

        // If there was only one item in this location, remove all of the
        // menu options other than 'jump to' (which in this case are the
        // separator and the single 'raise' command).
        if (proxies.length() == 1)
            while (menu->actions().back() != jump_to)
                menu->removeAction(menu->actions().back());

        // If we have items in the menu, run it and get the resulting action.
        QAction* chosen = (proxies.length() > 0) ? menu->exec(QCursor::pos())
                                                 : NULL;
        if (chosen && chosen != jump_to)
        {
            if (raised)
                raised->setZValue(0);
            raised = chosen->data().value<ControlProxy*>();
            raised->setZValue(0.1);
        }
        else if (proxies.length() == 0)
        {
            QMenu* m = new QMenu(this);

            Q_ASSERT(dynamic_cast<MainWindow*>(parent()));
            auto window = static_cast<MainWindow*>(parent());
            window->populateMenu(m, false);

            m->exec(QCursor::pos());
            m->deleteLater();
        }


        menu->deleteLater();
    }

    _dragging = false;
}

void Viewport::setYaw(float y)
{
    yaw = y;
    update();
    scene->invalidate(QRect(),QGraphicsScene::ForegroundLayer);
    emit(viewChanged());
}

void Viewport::setPitch(float p)
{
    pitch = p;
    update();
    scene->invalidate(QRect(),QGraphicsScene::ForegroundLayer);
    emit(viewChanged());
}

void Viewport::wheelEvent(QWheelEvent *event)
{
    QVector3D a = sceneToWorld(mapToScene(event->pos()));
    scale *= pow(1.001, -event->delta());
    QVector3D b = sceneToWorld(mapToScene(event->pos()));
    pan(a - b);
    emit(viewChanged());
    emit(scaleChanged(scale));
}

void Viewport::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    scene->invalidate(QRect(),QGraphicsScene::ForegroundLayer);
}


void Viewport::keyPressEvent(QKeyEvent *event)
{
    QGraphicsView::keyPressEvent(event);
    if (event->isAccepted())
        return;

    if (event->key() == Qt::Key_A &&
                (event->modifiers() & Qt::ShiftModifier))
    {
        QObject* w = this;
        while (!dynamic_cast<MainWindow*>(w))
            w = w->parent();
        Q_ASSERT(w);
        QMenu* m = new QMenu(static_cast<MainWindow*>(w));
        static_cast<MainWindow*>(w)->populateMenu(m, false, this);

        m->exec(QCursor::pos());
        delete m;
    }
}

void Viewport::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsView::keyPressEvent(event);
}

void Viewport::hideUI()
{
    ui_hidden = true;
    for (auto i : scene->items())
        if (auto c = dynamic_cast<ControlProxy*>(i))
            c->hide();
}

void Viewport::showUI()
{
    ui_hidden = false;
    for (auto i : scene->items())
        if (auto c = dynamic_cast<ControlProxy*>(i))
            c->show();
}

void Viewport::pan(QVector3D d)
{
    center += d;
    update();
    scene->invalidate(QRect(),QGraphicsScene::ForegroundLayer);
    emit(viewChanged());
    emit(centerChanged(center));
}

void Viewport::drawBackground(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(rect);

    painter->beginNativePainting();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto d : findChildren<DepthImageItem*>())
        d->paint();
    painter->endNativePainting();
}

void Viewport::drawAxes(QPainter* painter) const
{
    // First, draw the axes.
    auto m = getMatrix();
    QVector3D o = m * QVector3D(0, 0, 0);
    QVector3D x = m * QVector3D(1, 0, 0);
    QVector3D y = m * QVector3D(0, 1, 0);
    QVector3D z = m * QVector3D(0, 0, 1);

    QList<QPair<QVector3D, QColor>> pts = {
        {x, Colors::red},
        {y, Colors::green},
        {z, Colors::blue}};

    // Sort the axes to fake proper z clipping
    std::sort(pts.begin(), pts.end(),
            [](QPair<QVector3D, QColor> a, QPair<QVector3D, QColor> b)
            { return a.first.z() < b.first.z(); });

    for (auto p : pts)
    {
        painter->setPen(QPen(p.second, 2));
        painter->drawLine(o.toPointF(), p.first.toPointF());
    }
}

void Viewport::drawMousePosition(QPainter* painter) const
{
    // Then add a text label in the lower-left corner
    // giving mouse coordinates (if we're near an axis)
    if (getAxis().first)
    {
        QPair<char, float> axis = getAxis();
        QPointF mouse_pos = mapToScene(mapFromGlobal(QCursor::pos()));
        if (!sceneRect().contains(mouse_pos))
            axis.first = '\0';

        auto p = sceneToWorld(mouse_pos);

        QPointF a = sceneRect().bottomLeft() + QPointF(10, -25);
        QPointF b = sceneRect().bottomLeft() + QPointF(10, -10);
        int value = axis.second * 200;
        painter->setPen(QColor(value, value, value));
        if (axis.first == 'z')
        {
            painter->drawText(a, QString("X: %1").arg(p.x()));
            painter->drawText(b, QString("Y: %1").arg(p.y()));
        }
        else if (axis.first == 'y')
        {
            painter->drawText(a, QString("X: %1").arg(p.x()));
            painter->drawText(b, QString("Z: %1").arg(p.z()));
        }
        else if (axis.first == 'x')
        {
            painter->drawText(a, QString("Y: %1").arg(p.y()));
            painter->drawText(b, QString("Z: %1").arg(p.z()));
        }
    }
}

void Viewport::drawInfo(QPainter* painter) const
{
    /* top left view info 'panel' */
    painter->setPen(Colors::base04);
    QPointF top_left_info = sceneRect().topLeft();

    auto proxies = getProxiesAtPosition(_current_pos);

    if (proxies.size() > 0)
    {
        auto n = proxies.first()->getNode();
        QString desc = n->getName() + " (" + n->getTitle() + ")";

        if (proxies.size() > 1)
            painter->drawText(top_left_info + QPointF(10, 1*15),
                    QString("Current: %1, (%2 more below)").arg(desc).arg(proxies.size()-1));
        else
            painter->drawText(top_left_info + QPointF(10, 1*15), QString("Current: %1").arg(desc));
    }
    else
    {
        painter->drawText(top_left_info + QPointF(10, 1*15), QString("Current: <none>"));
    }

    /* display scale, pitch, and yaw */
    painter->drawText(top_left_info + QPointF(10, 2*15), QString("Scale: %1").arg(scale/100));
    painter->drawText(top_left_info + QPointF(10, 3*15), QString("Pitch: %1").arg(getPitch()));
    painter->drawText(top_left_info + QPointF(10, 4*15), QString("Yaw: %1").arg(getYaw()));
}

void Viewport::drawForeground(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(rect);

    drawAxes(painter);
    drawMousePosition(painter);
    drawInfo(painter);
}

void Viewport::onCopy()
{
    for (auto i : scene->selectedItems())
        if (auto proxy = dynamic_cast<ControlProxy*>(i))
        {
            auto n = proxy->getControl()->getNode();
            Q_ASSERT(dynamic_cast<NodeRoot*>(n->parent()));
            auto p = static_cast<NodeRoot*>(n->parent());

            NodeRoot temp_root;
            n->setParent(&temp_root);
            auto data = new QMimeData();
            data->setData("sb::viewport",
                    QJsonDocument(SceneSerializer(&temp_root).run()).toJson());
            n->setParent(p);

            QApplication::clipboard()->setMimeData(data);
            return;
        }
}

void Viewport::onCut()
{
    for (auto i : scene->selectedItems())
        if (auto proxy = dynamic_cast<ControlProxy*>(i))
        {
            auto n = proxy->getControl()->getNode();
            Q_ASSERT(dynamic_cast<NodeRoot*>(n->parent()));
            auto p = static_cast<NodeRoot*>(n->parent());

            NodeRoot temp_root;
            n->setParent(&temp_root);
            auto data = new QMimeData();
            data->setData("sb::viewport",
                    QJsonDocument(SceneSerializer(&temp_root).run()).toJson());
            n->setParent(p);

            QApplication::clipboard()->setMimeData(data);
            proxy->getControl()->deleteNode("'cut'");
            return;
        }
}

void Viewport::onPaste()
{
    auto data = QApplication::clipboard()->mimeData();
    if (data->hasFormat("sb::viewport"))
    {
        NodeRoot temp_root;
        SceneDeserializer ds(&temp_root);
        ds.run(QJsonDocument::fromJson(data->data("sb::viewport")).object());

        auto n = temp_root.findChild<Node*>();
        n->setParent(App::instance()->getNodeRoot());
        n->updateName();

        App::instance()->newNode(n);
        App::instance()->pushStack(new UndoAddNodeCommand(n, "'paste'"));
    }
}

void Viewport::onJumpTo(Node* n)
{
    auto proxies = getControlProxies(n);
    float area_sum = 0;
    if (!proxies.length())
        return;

    QVector3D pos;
    for (auto p : proxies)
    {
        const float area = p->boundingRect().width() *
                           p->boundingRect().height();
        pos += p->getControl()->pos() * area;
        area_sum += area;
    }
    pos /= area_sum;

    auto a = new QPropertyAnimation(this, "center");
    a->setDuration(100);
    a->setStartValue(center);
    a->setEndValue(pos);

    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void Viewport::setCenter(QVector3D c)
{
    center = c;
    update();
    scene->invalidate(QRect(), QGraphicsScene::ForegroundLayer);
    emit(viewChanged());
}

void Viewport::setScale(float s)
{
    scale = s;
    update();
    scene->invalidate(QRect(), QGraphicsScene::ForegroundLayer);
    emit(viewChanged());
}
