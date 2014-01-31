# coding=utf-8

import sys
import cPickle as pickle
import operator
import threading
import time

from PySide import QtCore, QtGui

import canvas
import button
################################################################################

class Window(QtGui.QMainWindow):
    def __init__(self, app, canvas):
        super(Window, self).__init__()
        self.setWindowTitle("Antimony")
        self.setCentralWidget(canvas)
        self.setGeometry(0, 900/4, 600, 400)
        self.make_menus(app)
        self.show()

    def make_menus(self, app):
         fileMenu = self.menuBar().addMenu("File");
         fileMenu.addAction(app.new_action)
         fileMenu.addAction(app.open_action)
         fileMenu.addAction(app.save_action)
         fileMenu.addAction(app.saveas_action)

         exportMenu = self.menuBar().addMenu("Export");
         exportMenu.addAction(app.export_stl_action)

         fileMenu.addAction(app.about_action)

class Container(QtGui.QWidget):
    """ Contains a single full-window widget;
        Used to stack buttons on top of a full-window canvas.
    """
    def __init__(self, widget):
        super(Container, self).__init__()
        self.widget = widget
        self.widget.setParent(self)

    def resizeEvent(self, event):
        self.widget.move(QtCore.QPoint())
        self.widget.resize(self.size())

################################################################################

class App(QtGui.QApplication):
    def __init__(self):
        super(App, self).__init__(sys.argv)
        self.setStyleSheet("""
        QGroupBox {
            background-color: #eee;
            border: 0px;
        }""")

        self.saved_state = None
        self.filename = None
        self.window = None

        self.make_actions()

        self.canvas = canvas.Canvas()
        container = Container(self.canvas)

        # Make a bunch of buttons and put them on top of the stack
        self.add_button = button.AddButton(
                container, self.add_object, -1)
        self.move_button = button.MoveButton(
                container, lambda b: self.set_mode(b, 'move'), 0)
        self.del_button = button.DelButton(
                container, lambda b: self.set_mode(b, 'delete'), 1)
        view_tool = ui.views.ViewTool(container, self.canvas.spin_to)

        for b in self.findChildren(button.Button) + [view_tool]:
            b.raise_()

        self.mode = 'move'
        self.move_button.selected = True

        self.window = Window(self, container)


        self.window.activateWindow()
        self.window.raise_()


    def add_object(self, button):
        """ Opens up a menu to add objects
            (centered on the given button).
        """
        if not self.canvas.openMenuAt(button.geometry().center()):
            return

        self.mode = 'move'
        if not self.move_button.selected:
            self.move_button.selected = True
            self.move_button.update()
        if self.del_button.selected:
            self.del_button.selected = False
            self.del_button.update()

    def set_mode(self, hit, mode):
        """ Sets self.mode to the given value
            and selects / deselects all buttons.
        """
        self.mode = mode
        for b in self.canvas.findChildren(button.Button):
            if b.selected != (b == hit):
                b.selected = (b == hit)
                b.update()


    def make_actions(self):
        """ Create a bunch of Qt actions with associated shortcuts.
        """
        self.new_action = QtGui.QAction("New", self)
        self.new_action.setShortcuts(QtGui.QKeySequence.New)
        self.new_action.triggered.connect(self.on_new)

        self.open_action = QtGui.QAction("Open", self)
        self.open_action.setShortcuts(QtGui.QKeySequence.Open)
        self.open_action.triggered.connect(self.on_open)

        self.save_action = QtGui.QAction("Save", self)
        self.save_action.setShortcuts(QtGui.QKeySequence.Save)
        self.save_action.triggered.connect(self.on_save)

        self.saveas_action = QtGui.QAction("Save As", self)
        self.saveas_action.setShortcuts(QtGui.QKeySequence.SaveAs)
        self.saveas_action.triggered.connect(self.on_saveas)

        self.about_action = QtGui.QAction("About", self)
        self.about_action.triggered.connect(self.on_about)

        self.export_stl_action = QtGui.QAction("Mesh (.stl)", self)
        self.export_stl_action.triggered.connect(self.on_export_stl)


    def on_about(self):
        QtGui.QMessageBox.about(self.canvas, "antimony",
                u"""<b>antimony</b><br><br>
                A non-traditional CAD tool.<br>
                <a href="https://github.com/mkeeter/antimony">https://github.com/mkeeter/antimony</a><br><br>
                © 2013 Matthew Keeter<br>
                <a href="mailto:matt.j.keeter@gmail.com">matt.j.keeter@gmail.com</a><br>
                _________________________________________________<br><br>
                Includes code from <a href="https://github.com/mkeeter/kokopelli">kokopelli</a>, which is <br>
                © 2012-2013 Massachusetts Institute of Technology<br>
                © 2013 Matthew Keeter<br><br>
                Inspired by the <a href="http://kokompe.cba.mit.edu">fab modules</a>
                """)

    def clear(self):
        """ Deletes all nodes, connections, and UI representations of same.
        """
        for n in node.base.nodes:
            n.delete()
        node.base.nodes = []

        # Delete all UI controls
        for n in self.canvas.findChildren(control.base.NodeControl):
            n.deleteLater()
        for c in self.canvas.findChildren(
                control.connection.ConnectionControl):
            c.deleteLater()
        for c in self.canvas.findChildren(ui.editor.Editor):
            c.deleteLater()

    def get_state(self):
        """ Return a serialized version of the scene
            (used in undo/redo and for saving/loading).
        """
        return (node.base.serialize_nodes(),
                node.connection.serialize_connections(node.base.nodes))

    def unsaved(self):
        return self.saved_state != self.get_state()

    def on_new(self):
        """ Creates a new file by clearing the old file.
        """
        self.clear()
        self.filename = None
        self.window.setWindowTitle("Antimony")

    def on_open(self):
        """ Opens a file, unpacking into a set of nodes and connections.
        """
        filename, filetype = QtGui.QFileDialog.getOpenFileName(
                self.window, "Open", '', "*.sb")
        if not filename:    return

        try:
            with open(filename, 'rb') as f:
                state = pickle.load(f)

                # Clear existing widgets
                self.clear()

                # Reconstruct all nodes
                node.base.load_nodes(state[0])

                # Then reconnect all of their connections
                node.connection.load_connections(state[1], node.base.nodes)
        except:
            print "Failed to load file"
            return

        # Make widgets for nodes and connections
        control.base.make_node_widgets(self.canvas)
        control.connection.make_connection_widgets(node.base.nodes, self.canvas)

        self.filename = filename
        self.window.setWindowTitle("Antimony [%s]" % self.filename)

    def on_save(self):
        """ Saves a pickled representation of our current state.
        """
        if not self.filename:
            return self.on_saveas()

        state = self.get_state()
        with open(self.filename, 'wb') as f:
            pickle.dump(state, f)

        self.window.setWindowTitle("Antimony [%s]" % self.filename)
        self.saved_state = state


    def on_saveas(self):
        """ Select a filename, then assign it to self.filename and
            call this class's on_save() function.
        """
        filename, filetype = QtGui.QFileDialog.getSaveFileName(
                self.window, "Save as", '', "*.sb")
        if filename:
            self.filename = filename
            return self.on_save()


    def on_export_stl(self):
        """ Exports as a .stl mesh
        """

        expressions = self.canvas.find_expressions()
        if not expressions:
            return QtGui.QMessageBox.critical(self.canvas, "Export error",
                    "<b>Export error:</b>" +
                    "<br><br>Must have one or more expressions.")

        # Merge all expressions into one for export
        combined = reduce(operator.or_, [e[1] for e in expressions])

        if not combined.has_xyz_bounds():
            return QtGui.QMessageBox.critical(self.canvas, "Export error",
                    "<b>Export error:</b>" +
                    "<br><br>All expressions must be 3D objects.")

        rd = ResolutionDialog(self.canvas,
                combined.xmax - combined.xmin,
                combined.ymax - combined.ymin,
                combined.zmax - combined.zmin)

        if not rd.exec_():  return
        res = rd.number.value()

        filename, filetype = QtGui.QFileDialog.getSaveFileName(
                self.window, "Export (.stl)", '', '*.stl')
        if not filename:    return

        self.block("Exporting",
                   lambda: combined.to_tree().triangulate(res, filename))


    def block(self, message, func):
        txt = QtGui.QLabel(message, self.canvas)

        txt.setGeometry(self.canvas.geometry())
        txt.setAlignment(QtCore.Qt.AlignCenter)
        txt.setStyleSheet("""
                background-color: rgba(100, 100, 100, 75%);
                color: #eee;
                font: 60px;
                """)
        txt.show()

        thread = threading.Thread(target=func)
        thread.daemon = True
        thread.start()
        dots = 0
        while True:
            if not thread.is_alive():
                thread.join()
                break
            pause = 1 if dots == 3 else 0.5
            txt.setText(message + '.'*dots + ' '*(3-dots))
            dots = (dots + 1) % 4
            for i in range(int(pause/0.05)):
                self.processEvents()
                time.sleep(0.05)
        txt.deleteLater()

################################################################################

class ResolutionDialog(QtGui.QDialog):
    def __init__(self, parent, dx, dy, dz):
        super(ResolutionDialog, self).__init__(parent)

        layout = QtGui.QFormLayout(self)
        self.setWindowTitle("Export .stl")

        label = QtGui.QLabel("<b>Resolution</b><br>(voxels/unit)", self)
        layout.addRow(label)

        self.number = QtGui.QDoubleSpinBox(self)
        self.number.setMinimum(0.01)
        self.number.setMaximum(10)
        self.number.setStyleSheet("width: 100px;")

        button = QtGui.QPushButton("Okay", self)
        button.clicked.connect(self.accept)

        layout.addRow(self.number, button)

        voxels = QtGui.QLabel('0 x 0 x 0', self)
        layout.addRow(voxels)
        self.setLayout(layout)

        self.number.valueChanged.connect(lambda v:
                voxels.setText('%i x %i x %i' %
                    (max(1, v*dx), max(1, v*dy), max(1, v*dz))))

        self.number.setValue(1)

################################################################################

import control.base, control.connection
import ui.editor
import node.base, node.connection
import ui.views
