#!/bin/env python

name = raw_input("Node base name (i.e. 'Circle')?  ")
category = raw_input("Node category (i.e. 'Meta')?  ")

control_types = ["Control", "DummyControl", "WireframeControl"]
control_type = control_types[int(raw_input("Control base class?\n"
    + '\n'.join("%i) %s" % (i + 1,s)
                for i, s in enumerate(control_types)) + '\n')) - 1]

################################################################################

# Write node header
open("src/node/{0}/{1}_node.h".format(category.lower(), name.lower()), 'wb').write(
"""#ifndef {0}_NODE
#define {0}_NODE

#include "node/node.h"

class {1}Node : public Node
{{
public:
    explicit {1}Node(QObject* parent=0);
    explicit {1}Node(float x, float y, float z, float scale,
                     QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        {{ return NodeType::{0}; }}
}};

#endif
""".format(name.upper(), name))

################################################################################

# Write node source
open("src/node/{0}/{1}_node.cpp".format(category.lower(), name.lower()), 'wb').write(
"""#include "node/{3}/{0}_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"
#include "datum/function_datum.h"
#include "datum/shape_datum.h"
#error "Must include relevant datums here"

{1}Node::{1}Node(QObject* parent)
    : Node(parent)
{{
    // Nothing to do here
}}

{1}Node::{1}Node(float x, float y, float z, float scale,
                     QObject* parent)
    : Node(NodeManager::manager()->getName("{2}"), parent)
{{
    #error "Must create relevant datums here"
}}
""".format(name.lower(), name, name[0].lower(), category.lower()))

################################################################################

# Write control header
open("src/control/{0}/{1}_control.h".format(category.lower(), name.lower()), 'wb').write(
"""#ifndef {0}_CONTROL_H
#define {0}_CONTROL_H

#include "control/{1}.h"

class {2}Control : public {3}
{{
public:
    explicit {2}Control(Canvas* canvas, Node* node);

    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
{4}
}};

#endif
""".format(name.upper(), control_type.lower(), name, control_type, {
"Control":
"""    QRectF bounds() const override;""",
"WireframeControl":
"""    QVector<QVector<QVector3D>> lines() const override;""",
"DummyControl":
"""    QRectF bounds() const override;""",
}[control_type]))

################################################################################

# Write control header
open("src/control/{0}/{1}_control.cpp".format(category.lower(), name.lower()), 'wb').write(
'''#include <Python.h>
#include "control/{0}/{1}_control.h"

#include "ui/canvas.h"

{2}Control::{2}Control(Canvas* canvas, Node* node)
    : {3}(canvas, node)
{{
    #error "datums need to be watched"
}}

void {2}Control::drag(QVector3D c, QVector3D d)
{{
    #error "drag is not implemented"
}}

QPointF {2}Control::inspectorPosition() const
{{
    #error "inspectorPosition is not implemented"
}}

{4}
'''.format(category, name.lower(), name, control_type, {
"Control":
"""QRectF {0}Control::bounds() const
{{
    #error "bounds is not implemented"
}}""".format(name),
'WireframeControl':
"""QVector<QVector<QVector3D>> {0}Control::lines() const
{{
    #error "lines is not implemented"
}}""".format(name),
"DummyControl":
"""QRectF {0}Control::bounds() const
{{
    #error "bounds is not implemented"
}}""".format(name),
}[control_type]))

################################################################################

# Add node to main window (menus)
main_window_file = "src/ui/main_window.cpp"
main_window_include = "// NODE HEADERS"
main_window_menu = "    // NODE CASES"

# Update main window
main_window = open(main_window_file, 'rb').read()
open(main_window_file, "wb").write(
    main_window.replace(main_window_include,
        '''#include "node/{0}/{1}_node.h"
'''.format(category.lower(), name.lower()) +
        main_window_include
    ).replace(main_window_menu,
        '''    addNodeToMenu<{0}Node>("{1}", "{0}", menu, &submenus);
'''.format(name, category) +
        main_window_menu)
)

################################################################################

# Add node to deserializer (loading)
deserializer_file = "src/node/deserializer.cpp"
deserializer_include = "// NODE HEADERS"
deserializer_case = "        // NODE CASES"

# Update deserializer
deserializer = open(deserializer_file, 'rb').read()
open(deserializer_file, 'wb').write(
    deserializer.replace(deserializer_include,
        '''#include "node/{0}/{1}_node.h"
'''.format(category.lower(), name.lower()) +
        deserializer_include
    ).replace(
        deserializer_case,
        '''        case NodeType::{0}:
            node = new {1}Node(p); break;
'''.format(name.upper(), name) +
        deserializer_case)
)

################################################################################

# Add node to manager (control construction)
manager_file = "src/node/manager.cpp"
manager_include = "// CONTROL HEADERS"
manager_case = "        // CONTROL CASES"

manager = open(manager_file, 'rb').read()
open(manager_file, 'wb').write(
    manager.replace(manager_include,
        '''#include "control/{0}/{1}_control.h"
'''.format(
            category.lower(), name.lower()) +
        manager_include
    ).replace(manager_case,
        '''        case NodeType::{0}:
            return new {1}Control(canvas, n);
'''.format(name.upper(), name) +
        manager_case)
)

################################################################################

types_file = "src/node/node_types.h"
types_case = "        // NODE TYPES"

types = open(types_file, 'rb').read()
open(types_file, 'wb').write(
        types.replace(types_case,
            '''        {0},
'''.format(name.upper()) + types_case)
)

################################################################################

# Add node to nodes.pri
nodes_file = "qt/nodes.pri"
nodes_header = "    # NODE HEADERS"
nodes_source = "    # NODE SOURCES"

nodes = open(nodes_file, 'rb').read()
open(nodes_file, 'wb').write(
    nodes.replace(nodes_header,
        '''    ../src/node/{0}/{1}_node.h \\
'''.format(category.lower(), name.lower()) + nodes_header
    ).replace(nodes_source,
        '''    ../src/node/{0}/{1}_node.cpp \\
'''.format(category.lower(), name.lower())+ nodes_source)
)

################################################################################

# Add control to controls.pri
controls_file = "qt/controls.pri"
controls_header = "    # CONTROL HEADERS"
controls_source = "    # CONTROL SOURCES"

controls = open(controls_file, 'rb').read()
open(controls_file, 'wb').write(
    controls.replace(controls_header,
        '''    ../src/control/{0}/{1}_control.h \\
'''.format(category.lower(), name.lower()) + controls_header
    ).replace(controls_source,
        '''    ../src/control/{0}/{1}_control.cpp \\
'''.format(category.lower(), name.lower()) + controls_source)
)

