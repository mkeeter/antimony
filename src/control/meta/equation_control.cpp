#include <boost/python.hpp>

#include "control/meta/equation_control.h"

#include "graph/node/node.h"
#include "graph/datum/datum.h"
#include "fab/types/shape.h"
#include "tree/node/printers_ss.h"

#include "ui/colors.h"

EquationControl::EquationControl(Canvas *canvas, Node *node)
    : TextLabelControl("Equation", Colors::violet, canvas, node)
{
    connect(node->getDatum("a"), SIGNAL(changed()),
            this, SLOT(redraw()));
}

QString EquationControl::getLabel() const
{
    PyObject* name = node->getDatum("a")->getValue();

    boost::python::extract<Shape> get_shape(name);
    Q_ASSERT(get_shape.check());
    Shape s = get_shape();

    return print_node_ss(s.tree->head).c_str();
}
