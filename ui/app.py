import sys
import cPickle as pickle

from PySide import QtCore, QtGui

import canvas
import button
################################################################################

class Window(QtGui.QMainWindow):
    def __init__(self, app, canvas):
        super(Window, self).__init__()
        self.setWindowTitle("Antimony")
        self.setCentralWidget(canvas)
        self.setGeometry(0, 900/4, 1440/2, 900/2)
        self.make_menus(app)
        self.show()

    def make_menus(self, app):
         fileMenu = self.menuBar().addMenu("File");
         fileMenu.addAction(app.new_action)
         fileMenu.addAction(app.open_action)
         fileMenu.addAction(app.save_action)
         fileMenu.addAction(app.saveas_action)

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

        self.add_button = button.AddButton(
                self.canvas, self.add_object, lambda y: y/2 - 55)
        self.move_button = button.MoveButton(
                self.canvas, lambda b: self.set_mode(b, 'move'),
                lambda y: y/2 - 15)
        self.del_button = button.DelButton(
                self.canvas, lambda b: self.set_mode(b, 'delete'),
                lambda y: y/2 + 25)
        self.mode = 'move'
        self.move_button.selected = True

        self.window = Window(self, self.canvas)

    def add_object(self, button):
        """ Opens up a menu to add objects
            (centered on the given button).
        """
        self.canvas.openMenuAt(button.geometry().center())
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

    def on_open(self):
        """ Opens a file, unpacking into a set of nodes and connections.
        """
        filename, filetype = QtGui.QFileDialog.getOpenFileName(
                self.window, "Open", '', "*.sb")
        if not filename:    return

        try:
            with open(filename, 'rb') as f:
                state = pickle.load(f)

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


    def on_save(self):
        """ Saves a pickled representation of our current state.
        """
        if not self.filename:
            return self.on_saveas()

        state = self.get_state()
        with open(self.filename, 'wb') as f:
            pickle.dump(state, f)
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


import control.base, control.connection
import ui.editor
import node.base, node.connection
