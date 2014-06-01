import types

from PySide import QtGui

from sb.canvas import Canvas
from sb.manager import NodeManager
from .ui_main_window import Ui_MainWindow

class MainWindow(QtGui.QMainWindow):
    def __init__(self):
        super(MainWindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.add_canvas()
        self.set_shortcuts()
        self.make_add_menu()

    def add_canvas(self):
        """ Adds the QGraphicsView that represents our scene.
        """
        self.canvas = Canvas()
        self.ui.gridLayout.addWidget(self.canvas, 0, 0)
        self.canvas.lower()

    def set_shortcuts(self):
        """ Sets keyboard shortcuts for UI actions
            (as Qt Designer doesn't have a way to do so).
        """
        self.ui.actionNew.setShortcuts(QtGui.QKeySequence.New)
        self.ui.actionOpen.setShortcuts(QtGui.QKeySequence.Open)
        self.ui.actionSave.setShortcuts(QtGui.QKeySequence.Save)
        self.ui.actionSaveAs.setShortcuts(QtGui.QKeySequence.SaveAs)
        self.ui.actionQuit.setShortcuts(QtGui.QKeySequence.Quit)

    def _put_in_add_menu(self, d):
        """ Adds the specific class to the 'Add' menu if it has
            menu_name and menu_category items.
        """
        if hasattr(d, 'menu_name') and hasattr(d, 'menu_category'):
            if not d.menu_category in self._categories:
                self._categories[d.menu_category] = self.ui.menuAdd.addMenu(
                        d.menu_category)
            act = self._categories[d.menu_category].addAction(d.menu_name)
            act.triggered.connect(lambda: self.create_new(d))

    def create_new(self, d):
        """ Creates a new instance of d, placing it at the center of
            the viewport and attaching it to the mouse.
        """
        # This is the mouse position in viewport coordinates
        mouse_pos = self.canvas.rect().center()
        # ...and in scene coordinates
        scene_pos = self.canvas.sceneRect().center()
        # ... and in world coordinates
        obj_pos = self.canvas.imatrix * QtGui.QVector3D(scene_pos)

        # Move the mouse to the center of the canvas
        QtGui.QCursor.setPos(self.canvas.mapToGlobal(mouse_pos))

        # Pick a unique name then create the node
        name = NodeManager.get_name(d)
        node = d.new(self.canvas, name,
                     obj_pos.x(), obj_pos.y(), obj_pos.z(),
                     100 / self.canvas._scale)

        # Stick this node's control to the mouse
        node.control._mouse_click_pos = scene_pos
        node.control._hover = True
        node.control.grabMouse

    def make_add_menu(self):
        import sb.nodes
        self._categories = {}
        for v in sb.nodes.__dict__.values():
            if isinstance(v, types.ModuleType) and 'sb.nodes' in v.__name__:
                for d in v.__dict__.values():
                    try:
                        sub = issubclass(d, sb.nodes.node.Node)
                    except:
                        sub = False

                    if sub:
                        self._put_in_add_menu(d)

