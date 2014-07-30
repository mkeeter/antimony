#include "node/2d/text_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"
#include "datum/string_datum.h"

TextNode::TextNode(QObject* parent)
    : Node(parent)
{
    // Nothing to do here
}

TextNode::TextNode(float x, float y, float z, float scale,
                   QObject* parent)
    : TextNode(parent)
{
    Q_UNUSED(z);
    new FloatDatum("_x", QString::number(x), this);
    new FloatDatum("_y", QString::number(y), this);
    new StringDatum("text", "hello", this);
    new FloatDatum("scale", QString::number(scale), this);
    new ShapeFunctionDatum("shape", this, "text", {"text", "_x", "_y", "scale"});
}
