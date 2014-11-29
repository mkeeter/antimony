#include <Python.h>

#include "ui/viewport/scene.h"
#include "ui/viewport/viewport.h"

#include "graph/node/node.h"
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
}

Viewport* ViewportScene::newViewport()
{
    auto s = new QGraphicsScene;
    auto v = new Viewport(s);
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
    auto p = new ControlProxy(c, v);
    scenes[v]->addItem(p);
}

void ViewportScene::makeRenderWorkersFor(Node* n, Viewport* v)
{
#warning "Not yet implemented"
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
}

Control* ViewportScene::makeControlFor(Node* n) const
{
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////

#if 0

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

#include "control/meta/script_control.h"
#include "control/meta/equation_control.h"

#include "control/csg/union_control.h"
#include "control/csg/blend_control.h"
#include "control/csg/intersection_control.h"
#include "control/csg/difference_control.h"
#include "control/csg/offset_control.h"
#include "control/csg/clearance_control.h"
#include "control/csg/shell_control.h"

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

#include "control/variable/slider_control.h"

#include "control/iterate/iterate2d_control.h"

Control* Control::makeControlFor(Canvas* canvas, Node* node)
{
   switch (node->getNodeType())
    {
        case NodeType::CIRCLE:
            return new CircleControl(canvas, node);
        case NodeType::TRIANGLE:
            return new TriangleControl(canvas, node);
        case NodeType::POINT2D:
            return new Point2DControl(canvas, node);
        case NodeType::RECTANGLE:
            return new RectangleControl(canvas, node);
        case NodeType::CUBE:
            return new CubeControl(canvas, node);
        case NodeType::SPHERE:
            return new SphereControl(canvas, node);
        case NodeType::CYLINDER:
            return new CylinderControl(canvas, node);
        case NodeType::CONE:
            return new ConeControl(canvas, node);
        case NodeType::EXTRUDE:
            return new ExtrudeControl(canvas, node);
        case NodeType::POINT3D:
            return new Point3DControl(canvas, node);
        case NodeType::SCRIPT:
            return new ScriptControl(canvas, node);
        case NodeType::EQUATION:
            return new EquationControl(canvas, node);
        case NodeType::TEXT:
            return new TextControl(canvas, node);
        case NodeType::UNION:
            return new UnionControl(canvas, node);
        case NodeType::BLEND:
            return new BlendControl(canvas, node);
        case NodeType::INTERSECTION:
            return new IntersectionControl(canvas, node);
        case NodeType::DIFFERENCE:
            return new DifferenceControl(canvas, node);
        case NodeType::OFFSET:
            return new OffsetControl(canvas, node);
        case NodeType::CLEARANCE:
            return new ClearanceControl(canvas, node);
        case NodeType::SHELL:
            return new ShellControl(canvas, node);
        case NodeType::ATTRACT:
            return new AttractControl(canvas, node);
        case NodeType::REPEL:
            return new RepelControl(canvas, node);
        case NodeType::SCALEX:
            return new ScaleXControl(canvas, node);
        case NodeType::SCALEY:
            return new ScaleYControl(canvas, node);
        case NodeType::SCALEZ:
            return new ScaleZControl(canvas, node);
        case NodeType::ROTATEX:
            return new RotateXControl(canvas, node);
        case NodeType::ROTATEY:
            return new RotateYControl(canvas, node);
        case NodeType::ROTATEZ:
            return new RotateZControl(canvas, node);
        case NodeType::REFLECTX:
            return new ReflectXControl(canvas, node);
        case NodeType::REFLECTY:
            return new ReflectYControl(canvas, node);
        case NodeType::REFLECTZ:
            return new ReflectZControl(canvas, node);
        case NodeType::RECENTER:
            return new RecenterControl(canvas, node);
        case NodeType::TRANSLATE:
            return new TranslateControl(canvas, node);
        case NodeType::SLIDER:
            return new SliderControl(canvas, node);
        case NodeType::ITERATE2D:
            return new Iterate2DControl(canvas, node);
    }
   Q_ASSERT(false);
   return NULL;
}

#endif
