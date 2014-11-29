#include <Python.h>

#include <QGraphicsSceneMouseEvent>
#include <QRegularExpression>

#include "control/control.h"

#include "ui/viewport/viewport.h"
#include "ui/colors.h"

#include "graph/node/node.h"
#include "graph/datum/datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/script_datum.h"

Control::Control(Node* node, QObject* parent)
    : QObject(parent), node(node)
{
#if 0
    if (parent == NULL)
    {
        canvas->scene->addItem(this);
    }
#endif

    if (node)
        connect(node, SIGNAL(destroyed()), this, SLOT(deleteLater()));
}

QPainterPath Control::shape(QMatrix4x4 m) const
{
    QPainterPath p;
    p.addRect(bounds(m));
    return p;
}

Node* Control::getNode() const
{
    return node;
}

void Control::watchDatums(QVector<QString> datums)
{
    for (auto n : datums)
    {
        Datum* d = node->getDatum(n);
        Q_ASSERT(d);
        connect(d, &Datum::changed, this, &Control::redraw);
    }
}

double Control::getValue(QString name) const
{
    Datum* d = node->getDatum(name);
    Q_ASSERT(d);

    double v = PyFloat_AsDouble(d->getValue());
    Q_ASSERT(!PyErr_Occurred());

    return v;
}

void Control::deleteNode()
{
    if (parent())
        dynamic_cast<Control*>(parent())->deleteNode();
    else
        node->deleteLater();
}

void Control::dragValue(QString name, double delta)
{
    Datum* d = node->getDatum(name);
    Q_ASSERT(d);

    FloatDatum* f = dynamic_cast<FloatDatum*>(d);
    Q_ASSERT(f);

    bool ok = false;
    QString s = f->getExpr();
    double v = s.toFloat(&ok);
    if (ok)
    {
        f->setExpr(QString::number(v + delta));
        return;
    }

    QRegularExpression regex(
        "(.*[+\\-]\\s*)(\\d*(\\.\\d*|)(e\\d+(\\.\\d*|)|))"
    );
    auto match= regex.match(s);
    if (match.isValid())
    {
        v = match.captured(2).toFloat(&ok);
        if (ok)
        {
            f->setExpr(match.captured(1) +
                       QString::number(v + delta));
        }
    }
}

void Control::setValue(QString name, double new_value)
{
    Datum* d = node->getDatum(name);
    Q_ASSERT(d);

    FloatDatum* f = dynamic_cast<FloatDatum*>(d);
    Q_ASSERT(f);

    bool ok = false;
    f->getExpr().toFloat(&ok);
    if (ok)
        f->setExpr(QString::number(new_value));
}

QColor Control::defaultPenColor() const
{
    return Colors::base04;
}

QColor Control::defaultBrushColor() const
{
    return Colors::dim(defaultPenColor());
}

void Control::setDefaultPen(bool highlight, QPainter *painter) const
{
    if (highlight)
        painter->setPen(QPen(Colors::highlight(defaultPenColor()), 2));
    else
        painter->setPen(QPen(defaultPenColor(), 2));
}

void Control::setDefaultBrush(bool highlight, QPainter *painter) const
{
    if (highlight)
        painter->setBrush(QBrush(Colors::highlight(defaultBrushColor())));
    else
        painter->setBrush(QBrush(defaultBrushColor()));
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
