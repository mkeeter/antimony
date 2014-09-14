#include <Python.h>

#include <algorithm>

#include <QGraphicsSceneMouseEvent>
#include <QRegularExpression>

#include "control/control.h"
#include "node/node.h"

#include "ui/canvas.h"
#include "ui/colors.h"
#include "ui/inspector/inspector.h"
#include "ui/port.h"

#include <QDebug>

#include "datum/datum.h"
#include "datum/float_datum.h"

Control::Control(Canvas* canvas, Node* node, QGraphicsItem* parent)
    : QGraphicsObject(parent), canvas(canvas), node(node), inspector(NULL),
      _hover(false), _dragged(false)
{
    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIgnoresTransformations);
    setAcceptHoverEvents(true);

    if (parent == NULL)
    {
        canvas->scene->addItem(this);
    }
    setZValue(1);
    connect(canvas, SIGNAL(viewChanged()),
            this, SIGNAL(portPositionChanged()));

    connect(canvas, &Canvas::viewChanged,
            this, &Control::redraw);

    if (node)
    {
        connect(node, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    }
    makePorts();
}

QRectF Control::boundingRect() const
{
    return node ? bounds() : QRectF();
}

QRectF Control::boundingBox(QVector<QVector3D> points, int padding) const
{
    float xmin =  INFINITY;
    float xmax = -INFINITY;
    float ymin =  INFINITY;
    float ymax = -INFINITY;

    for (auto p : points)
    {
        QPointF o = canvas->worldToScene(p);
        if (o.x() < xmin)   xmin = o.x();
        if (o.x() > xmax)   xmax = o.x();
        if (o.y() < ymin)   ymin = o.y();
        if (o.y() > ymax)   ymax = o.y();
    }

    return QRectF(xmin - padding, ymin - padding,
                  xmax - xmin + 2*padding,
                  ymax - ymin + 2*padding);
}

Node* Control::getNode() const
{
    return node;
}

QPointF Control::baseOutputPosition() const
{
    return inspectorPosition();
}

QPointF Control::baseInputPosition() const
{
    return inspectorPosition();
}

QPointF Control::datumOutputPosition(Datum *d) const
{
    if (inspector)
    {
        OutputPort* p = inspector->datumOutputPort(d);
        if (p)
        {
            return (inspector->getMaskSize() *
                        p->mapToScene(p->boundingRect().center())) +
                    (1 - inspector->getMaskSize()) *
                        baseOutputPosition();
        }
    }
    return baseOutputPosition();
}

QPointF Control::datumInputPosition(Datum *d) const
{
    if (inspector)
    {
        InputPort* p = inspector->datumInputPort(d);
        if (p)
        {
            return (inspector->getMaskSize() *
                        p->mapToScene(p->boundingRect().center())) +
                    (1 - inspector->getMaskSize()) *
                        baseInputPosition();
        }
    }
    return baseInputPosition();
}

void Control::clearPorts()
{
    for (auto p : inputs)
        p->deleteLater();
    for (auto p : outputs)
        p->deleteLater();
    inputs.clear();
    outputs.clear();
}

void Control::makePorts()
{
    clearPorts();
    for (Datum* d : node->findChildren<Datum*>(QString(),
                Qt::FindDirectChildrenOnly))
    {
        if (d->hasInput())
            inputs << new InputPort(d, canvas);
        if (d->hasOutput())
            outputs << new OutputPort(d, canvas);
    }

    positionPorts();
    for (auto i : inputs)
        canvas->scene->addItem(i);
    for (auto o : outputs)
        canvas->scene->addItem(o);
}

void Control::positionPorts()
{
    QPointF p = baseInputPosition();
    const float step = 15;
    float y = -inputs.length()/2.0f * step;
    for (auto i : inputs)
    {
        i->setPos(p.x() - 25, p.y() + y + 5);
        y += step;
    }

    p = baseOutputPosition();
    y = -outputs.length()/2.0f * step;
    for (auto o : outputs)
    {
        o->setPos(p.x() + 15, p.y() + y + 5);
        y += step;
    }
}

void Control::watchDatums(QVector<QString> datums)
{
    for (auto n : datums)
    {
        Datum* d = node->getDatum(n);
        Q_ASSERT(d);
        connect(d, SIGNAL(changed()), this, SLOT(redraw()));
    }
}

void Control::redraw()
{
    prepareGeometryChange();
    positionPorts();
    if (node)
    {
        emit(inspectorPositionChanged());
        emit(portPositionChanged());
    }
}

void Control::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (!_hover)
    {
        _hover = true;
        update();
    }
}

void Control::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (_hover)
    {
        _hover = false;
        update();
    }
}

void Control::toggleInspector(bool show_hidden)
{
    if (parentObject() && dynamic_cast<Control*>(parentObject())->getNode() == node)
    {
        dynamic_cast<Control*>(parentObject())->toggleInspector();
    }
    else if (inspector.isNull())
    {
        inspector = new NodeInspector(this, show_hidden);
        connect(inspector, SIGNAL(portPositionChanged()),
                this, SIGNAL(portPositionChanged()));
        connect(inspector, SIGNAL(destroyed()),
                this, SIGNAL(portPositionChanged()));
    }
    else
    {
        inspector->animateClose();
    }
}

bool Control::showConnections() const
{
    return _hover || inspector || isSelected();
}

void Control::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }

    toggleInspector(event->modifiers() & Qt::ShiftModifier);
}

void Control::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        _dragged = false;
        _click_pos = event->pos();
    }
    else
    {
        event->ignore();
    }
}

void Control::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !_dragged)
    {
        setSelected(true);
    }
    else
    {
        event->ignore();
    }
    ungrabMouse();
}

void Control::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QVector3D p0 = canvas->sceneToWorld(_click_pos);
    QVector3D p1 = canvas->sceneToWorld(event->pos());

    drag(p1, p1 - p0);
    canvas->update();
    _click_pos = event->pos();
    _dragged = true;
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
    if (parentObject())
    {
        dynamic_cast<Control*>(parentObject())->deleteNode();
    }
    else
    {
        node->deleteLater();
    }
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
    {
        f->setExpr(QString::number(new_value));
    }
}

QColor Control::defaultPenColor() const
{
    return Colors::base04;
}

QColor Control::defaultBrushColor() const
{
    return Colors::dim(defaultPenColor());
}

void Control::setDefaultPen(QPainter *painter) const
{
    if (isSelected() or _hover)
    {
        painter->setPen(QPen(Colors::highlight(defaultPenColor()), 2));
    }
    else
    {
        painter->setPen(QPen(defaultPenColor(), 2));
    }
}

void Control::setDefaultBrush(QPainter *painter) const
{
    if (isSelected() or _hover)
    {
        painter->setBrush(QBrush(Colors::highlight(defaultBrushColor())));
    }
    else
    {
        painter->setBrush(QBrush(defaultBrushColor()));
    }
}

void Control::paint(QPainter *painter,
                    const QStyleOptionGraphicsItem *option,
                    QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (node)
    {
        paintControl(painter);
    }
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

////////////////////////////////////////////////////////////////////////////////

DummyControl::DummyControl(Canvas *canvas, Node *node, QGraphicsItem *parent)
    : Control(canvas, node, parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setAcceptHoverEvents(false);
}

void DummyControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    Q_UNUSED(delta);

    // A DummyControl should never be dragged.
    Q_ASSERT(false);
}

QPainterPath DummyControl::shape() const
{
    return QPainterPath();
}

void DummyControl::paintControl(QPainter *painter)
{
    Q_UNUSED(painter);
}
