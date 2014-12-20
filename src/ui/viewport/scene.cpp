#include <Python.h>

#include "ui/viewport/scene.h"
#include "ui/viewport/viewport.h"
#include "render/render_worker.h"

#include "graph/node/node.h"
#include "graph/datum/datum.h"
#include "control/control.h"
#include "control/proxy.h"

ViewportScene::ViewportScene(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here
}

Control* ViewportScene::makeUIfor(Node* n)
{
    auto c = makeControlFor(n);
    controls[n] = c;

    for (auto itr = scenes.begin(); itr != scenes.end(); ++itr)
    {
        makeProxyFor(c, itr.key());
        makeRenderWorkersFor(n, itr.key());
    }

    // Behold, the wonders of C++11 and Qt5:
    connect(n, &Node::datumsChanged,
            [=]{ this->onDatumsChanged(n); });
    return c;
}

Viewport* ViewportScene::newViewport()
{
    auto s = new QGraphicsScene;
    auto v = new Viewport(s);
    connect(v, &QObject::destroyed, s, &QObject::deleteLater);
    connect(s, &QObject::destroyed, this, &ViewportScene::prune);
    scenes[v] = s;

    prune();

    for (auto itr = controls.begin(); itr != controls.end(); ++itr)
    {
        makeProxyFor(itr.value(), v);
        makeRenderWorkersFor(itr.key(), v);
    }

    return v;
}

void ViewportScene::makeProxyFor(Control* c, Viewport* v)
{
    if (!c)
        return;

    auto p = new ControlProxy(c, v);
    scenes[v]->addItem(p);
    connect(v, &Viewport::viewChanged,
            p, &ControlProxy::redraw);
    connect(c, &ControlProxy::destroyed,
            this, &ViewportScene::prune);

    for (auto f : c->findChildren<Control*>("", Qt::FindDirectChildrenOnly))
        makeProxyFor(f, v);
}

void ViewportScene::makeRenderWorkerFor(Datum* d, Viewport* v)
{
    auto w = new RenderWorker(d, v);
    workers[d] << w;
    connect(w, &RenderWorker::destroyed,
            this, &ViewportScene::prune);
}

void ViewportScene::makeRenderWorkersFor(Node* n, Viewport* v)
{
    for (auto d : n->findChildren<Datum*>())
        if (RenderWorker::accepts(d))
            makeRenderWorkerFor(d, v);
}

void ViewportScene::prune()
{
    QMap<QPointer<Node>, Control*> new_controls;

    for (auto itr = controls.begin(); itr != controls.end(); ++itr)
        if (itr.key())
            new_controls[itr.key()] = itr.value();
    controls = new_controls;

    QMap<QPointer<Viewport>, QGraphicsScene*> new_scenes;
    for (auto itr = scenes.begin(); itr != scenes.end(); ++itr)
        if (itr.key())
            new_scenes[itr.key()] = itr.value();
    scenes = new_scenes;

    decltype(workers) new_workers;
    for (auto itr = workers.begin(); itr != workers.end(); ++itr)
        if (itr.key())
            for (auto w : workers[itr.key()])
                if (w)
                    new_workers[itr.key()] << w;
    new_workers = workers;
}

void ViewportScene::onDatumsChanged(Node* n)
{
    prune();

    for (auto d : n->findChildren<Datum*>())
        if (RenderWorker::accepts(d) && !workers.contains(d))
            for (auto v = scenes.begin(); v != scenes.end(); ++v)
                makeRenderWorkerFor(d, v.key());
}


////////////////////////////////////////////////////////////////////////////////


#include "control/2d/circle_control.h"
#include "control/2d/triangle_control.h"
#include "control/2d/text_control.h"
#include "control/2d/point2d_control.h"
#include "control/2d/rectangle_control.h"

#include "control/3d/cube_control.h"
#include "control/3d/sphere_control.h"
#include "control/3d/cylinder_control.h"
#include "control/3d/cone_control.h"
#include "control/3d/extrude_control.h"
#include "control/3d/point3d_control.h"

#include "control/deform/attract_control.h"
#include "control/deform/repel_control.h"
#include "control/deform/scalex_control.h"
#include "control/deform/scaley_control.h"
#include "control/deform/scalez_control.h"

#include "control/transform/rotatex_control.h"
#include "control/transform/rotatey_control.h"
#include "control/transform/rotatez_control.h"
#include "control/transform/reflectx_control.h"
#include "control/transform/reflecty_control.h"
#include "control/transform/reflectz_control.h"
#include "control/transform/recenter_control.h"
#include "control/transform/translate_control.h"

#include "control/iterate/iterate2d_control.h"

//#include "control/variable/slider_control.h"

Control* ViewportScene::makeControlFor(Node* node) const
{
   switch (node->getNodeType())
    {
        case NodeType::CIRCLE:
            return new CircleControl(node);
        case NodeType::TRIANGLE:
            return new TriangleControl(node);
        case NodeType::POINT2D:
            return new Point2DControl(node);
        case NodeType::RECTANGLE:
            return new RectangleControl(node);
        case NodeType::CUBE:
            return new CubeControl(node);
        case NodeType::SPHERE:
            return new SphereControl(node);
        case NodeType::CYLINDER:
            return new CylinderControl(node);
        case NodeType::CONE:
            return new ConeControl(node);
        case NodeType::EXTRUDE:
            return new ExtrudeControl(node);
        case NodeType::POINT3D:
            return new Point3DControl(node);
        case NodeType::TEXT:
            return new TextControl(node);
        case NodeType::ATTRACT:
            return new AttractControl(node);
        case NodeType::REPEL:
            return new RepelControl(node);
        case NodeType::SCALEX:
            return new ScaleXControl(node);
        case NodeType::SCALEY:
            return new ScaleYControl(node);
        case NodeType::SCALEZ:
            return new ScaleZControl(node);
        case NodeType::ROTATEX:
            return new RotateXControl(node);
        case NodeType::ROTATEY:
            return new RotateYControl(node);
        case NodeType::ROTATEZ:
            return new RotateZControl(node);
        case NodeType::REFLECTX:
            return new ReflectXControl(node);
        case NodeType::REFLECTY:
            return new ReflectYControl(node);
        case NodeType::REFLECTZ:
            return new ReflectZControl(node);
        case NodeType::RECENTER:
            return new RecenterControl(node);
        case NodeType::TRANSLATE:
            return new TranslateControl(node);
        case NodeType::ITERATE2D:
            return new Iterate2DControl(node);
        case NodeType::SLIDER:
        case NodeType::UNION:
        case NodeType::BLEND:
        case NodeType::INTERSECTION:
        case NodeType::DIFFERENCE:
        case NodeType::OFFSET:
        case NodeType::CLEARANCE:
        case NodeType::SHELL:
        case NodeType::SCRIPT:
        case NodeType::EQUATION:
            return NULL;
    }
   Q_ASSERT(false);
   return NULL;
}
