#include <Python.h>

#include <algorithm>

#include <QMouseEvent>
#include <QClipboard>
#include <QDebug>
#include <QPropertyAnimation>
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QMimeData>
#include <QMenu>

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
      gl_initialized(false)
{
    setScene(scene);
    setStyleSheet("QGraphicsView { border-style: none; }");

    setSceneRect(-width()/2, -height()/2, width(), height());
    setRenderHints(QPainter::Antialiasing);

    auto gl = new QOpenGLWidget(this);
    setViewport(gl);
}

void Viewport::setupUI(Ui::MainWindow* ui)
{
    connect(ui->actionShaded, &QAction::triggered,
            [&]{ scene->invalidate(); });
    connect(ui->actionHeightmap, &QAction::triggered,
            [&]{ scene->invalidate(); });

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

void Viewport::makeNodeAtCursor(NodeConstructor f)
{
    QPointF scene_pos = mapToScene(mapFromGlobal(QCursor::pos()));
    QVector3D p = sceneToWorld(scene_pos);

    auto n = f(p.x(), p.y(), p.z(), 100 / scale, App::instance()->getNodeRoot());

    App::instance()->newNode(n);
    App::instance()->pushStack(new UndoAddNodeCommand(n));

    if (auto proxy = getControlProxy(n))
    {
        proxy->setClickPos(scene_pos);
        proxy->grabMouse();
    }
}

float Viewport::getZmax() const
{
    float zmax = -INFINITY;
    for (auto i : scene->items())
        if (DepthImageItem* p = dynamic_cast<DepthImageItem*>(i))
            zmax = fmax((getTransformMatrix() * p->pos).z() + p->size.z()/2,
                        zmax);
    return zmax;
}

float Viewport::getZmin() const
{
    float zmin = INFINITY;
    for (auto i : scene->items())
        if (DepthImageItem* p = dynamic_cast<DepthImageItem*>(i))
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

ControlProxy* Viewport::getControlProxy(Node* n)
{
    for (auto i : items())
    {
        auto p = dynamic_cast<ControlProxy*>(i);
        if (p && p->getNode() == n && !p->getControl()->parent())
            return p;
    }
    return NULL;
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
    // On right-click, show a menu of items to raise.
    if (event->button() == Qt::RightButton)
    {
        auto menu = new QMenu(this);
        int overlapping = 0;
        QSet<ControlProxy*> used;
        for (auto i : items(event->pos()))
        {
            while (i->parentItem())
                i = i->parentItem();

            auto c = dynamic_cast<ControlProxy*>(i);
            if (c && !used.contains(c))
            {
                overlapping++;
                auto n = c->getNode();
                auto a = new QAction(
                        n->getName() + " (" + n->getType() + ")", menu);
                a->setData(QVariant::fromValue(i));
                menu->addAction(a);
                used << c;
            }
        }
        if (overlapping > 1)
        {
            QAction* chosen = menu->exec(QCursor::pos());
            if (chosen)
            {
                if (raised)
                    raised->setZValue(1);
                raised = static_cast<ControlProxy*>(
                        chosen->data().value<QGraphicsItem*>());
                raised->setZValue(1.1);
            }
        }
        menu->deleteLater();
    }

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

    // If we're on an axis (which means the viewport is showing mouse
    // coordinates), force a redraw on mouse motion.
    if (getAxis().first)
        scene->invalidate(QRect(),QGraphicsScene::ForegroundLayer);
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

    if (event->key() == Qt::Key_Alt)
    {
        hideUI();
    }
    else if (event->key() == Qt::Key_A &&
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
    if (event->isAccepted())
        return;

    if (event->key() == Qt::Key_Alt)
        showUI();
}

void Viewport::hideUI()
{
    for (auto i : scene->items())
        if (auto c = dynamic_cast<ControlProxy*>(i))
            c->hide();
}

void Viewport::showUI()
{
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
}

void Viewport::drawBackground(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(painter);
    Q_UNUSED(rect);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Viewport::drawForeground(QPainter* painter, const QRectF& rect)
{
    Q_UNUSED(rect);

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

    // Then add a text label in the lower-left corner
    // giving mouse coordinates (if we're near an axis)
    QPair<char, float> axis = getAxis();
    QPointF mouse_pos = mapToScene(mapFromGlobal(QCursor::pos()));
    if (!sceneRect().contains(mouse_pos))
        axis.first = '\0';

    auto p = sceneToWorld(mouse_pos);

    QPointF a = sceneRect().bottomLeft() + QPointF(10, -25);
    QPointF b = sceneRect().bottomLeft() + QPointF(10, -10);
    painter->setPen(QColor(axis.second*200, axis.second*200, axis.second*200));
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

void Viewport::onCopy()
{
    for (auto i : scene->selectedItems())
        if (auto proxy = dynamic_cast<ControlProxy*>(i))
        {
            auto n = proxy->getControl()->getNode();
            auto p = n->parent();

            NodeRoot temp_root;
            n->setParent(&temp_root);
            auto data = new QMimeData();
            data->setData("sb::viewport", SceneSerializer(&temp_root).run());
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
            auto p = n->parent();

            NodeRoot temp_root;
            n->setParent(&temp_root);
            auto data = new QMimeData();
            data->setData("sb::viewport", SceneSerializer(&temp_root).run());
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
        ds.run(data->data("sb::viewport"));

        auto n = temp_root.findChild<Node*>();
        n->setParent(App::instance()->getNodeRoot());

        App::instance()->newNode(n);
        App::instance()->pushStack(new UndoAddNodeCommand(n, "'paste'"));
    }
}
