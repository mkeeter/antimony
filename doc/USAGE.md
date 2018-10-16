User's Guide to Antimony
========================

Antimony is a tool for computer-aided design (CAD).

Window types
------------

There are three window types, each of which gives a different perspective on the scene.

### Graph window
The graph window shows a graph representation of the current model.

Pan with the left mouse button;
zoom with the mouse wheel.

When you first start designing, the window should be empty.
Add new nodes either with the *Add* menu or by pressing Shift+A.

Each node has a name and one or more data values.
The name is in the upper left corner and can be edited;
data values are inside of the box.

Each datum's text field is evaluated as a chunk of Python code.
These code snippets are executed in a global namespace that includes nodes by name.
For example, given a Point node named `p0`,
here are valid values for its `y` datum:
- `1`
- `2 + 3`
- `p0.x * 2`

If a code snippet is invalid, the text field's border will turn red.
Hover over it with the mouse to see the Python traceback.

Shift + left-click on a datum's text field and drag right or left to slide values up and down.
Hold `alt` while doing so to adjust the value faster.

Datums can be connected using the I/O ports on the right and left.
Click and drag on the right-hand port to start a connection;
release the mouse button on a left-hand port to finish it.
While dragging, pressing Spacebar will snap the connection to the nearest valid port.

Any shape datum with an unconnected output port will be rendered.

### 3D viewport
The 3D viewport shows a 2D or 3D view of the current model.
It is rendered and refined in real time.

Pan with the left mouse button;
rotate with the right;
zoom with the mouse wheel.
The axis selector in the top right snaps the view to a particular axis.
When looking along a main axis,
the mouse pointer's coordinates are shown in the bottom left.

As in the graph window,
nodes can be added from the *Add* menu or with Shift+A.

Certain types of nodes put controls into this viewport.
For example, the default Circle node places a center-point and radius in the viewport.
These controls can be dragged with the left mouse button.
If multiple controls are overlapping,
right-clicking will open up a list and one can be chosen to raise above the others.

### Script editor
A script editor is used to edit the Python code of a Script node.
To open the script editor,
add a Script node in the graph view
then click on the icon (three horizontal lines)
in the top left of the inspector.

There are three panes in the editor window:
- The top pane is the script
- The middle pane is any output sent to `stdout` (e.g. with `print`)
- The bottom pane is any Python error that occurred during execution

The lower two panes only appear when needed
(i.e. when something was sent to `stdout`
or an error occurred respectively).

When an error occurs, the relevant line will be highlighted in red.

Exporting files
---------------
The export workflow in Antimony is a bit non-traditional.

To export a particular shape, create a node from the Export category then
connect the target shape to its input port.  If the shape is a valid target
for export, a export button will appears below the datums in the node;
clicking on this arrow will start the export task.

This means that multiple export tasks can be defined for different parts of
a model; details like resolution and even target filename can also be
hard-coded by editing the export node's script.
