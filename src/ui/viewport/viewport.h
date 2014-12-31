#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QPointer>

class Node;
class ControlProxy;
class InputPort;
class NodeInspector;
class ViewSelector;
class Link;

class Viewport : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Viewport(QGraphicsScene* scene, QWidget* parent=0);

    /** Returns our scale + rotation +translate transform matrix.
     */
    QMatrix4x4 getMatrix() const;

    /** Returns a transform matrix (without pixel scale applied)
     */
    QMatrix4x4 getTransformMatrix() const;

    /** Transforms points from world to scene coordinates.
     */
    QPointF worldToScene(QVector3D v) const;
    QVector<QPointF> worldToScene(QVector<QVector3D> v) const;

    /** Transforms points from scene to world coordinates.
     */
    QVector3D sceneToWorld(QPointF p) const;

    /** Look up scale.
     */
    float getScale() const { return scale; }

    /** Gets the minimum z position of a DepthImageItem.
     */
    float getZmin() const;

    /** Gets the maximum z position of a DepthImageItem.
     */
    float getZmax() const;

    /** Get yaw (in radians)
     */
    float getYaw() const { return yaw; }

    /** Get pitch (in radians)
     */
    float getPitch() const { return pitch; }

    /** Returns the viewpoint's center.
     */
    QVector3D getCenter() const { return center; }

    /*
     *  Locks yaw and pitch to the given values.
     */
    void lockAngle(float y, float p);

    /*
     *  Hides the ViewSelector tool.
     */
    void hideViewSelector();

    /*
     *  Look up which top-level ControlProxy is attached to the given node.
     */
    ControlProxy* getControlProxy(Node* n);

    /*
     *  Getter functions so that DepthImageItems can use these shared objects.
     */
    QOpenGLBuffer* getQuadVertices();
    QOpenGLShaderProgram* getShadedShader();
    QOpenGLShaderProgram* getHeightmapShader();

    QGraphicsScene* scene;
signals:
    void viewChanged();
    void showPorts(bool);

public slots:
    void onCopy();
    void onCut();
    void onPaste();

    void spinTo(float new_yaw, float new_pitch);

protected:
    /*
     *  Detects if we're near a major axis.  Returns a tuple
     *  ({'x'|'y'|'z'}, o) where o is the opacity at which
     *  text should be drawn.
     */
    QPair<char, float> getAxis() const;

    /*
     *  Create the vertices buffer for the height-map quad
     *  and the shader objects for shaded and height-map rendering.
     */
    void initializeGL();

    /** On mouse press, save mouse down position in _click_pos.
     *
     *  Left-clicks are saved in scene coordinates; right-clicks
     *  are saved in pixel coordinates.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /** Pan or spin the view.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /** On mouse wheel action, zoom about the mouse cursor.
     */
    void wheelEvent(QWheelEvent *event) override;

    /*
     *  When the mouse leaves the window, redraw
     *  (to clear cursor position text)
     */
    void leaveEvent(QEvent* event) override;

    /** On delete key press, delete nodes.
     */
    void keyPressEvent(QKeyEvent *event) override;

    /** On key release, show UI if it was alt.
     */
    void keyReleaseEvent(QKeyEvent *event) override;

    /** Clear the background (color and depth buffers)
     */
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    /*
     *  Draws the x, y, z axes.
     */
    void drawForeground(QPainter* painter, const QRectF& rect) override;

    /** Pans the scene rectangle.
     */
    void pan(QVector3D d);

    /** On resize, reposition the view selector menu
     */
    void resizeEvent(QResizeEvent* e);

    /** Hides all Control UI elements.
     */
    void hideUI();

    /** Shows all Control UI elements.
     */
    void showUI();

    QVector3D center;
    float scale;

    // Properties to automatically animate yaw and pitch (in radians)
    void setYaw(float y);
    Q_PROPERTY(float _yaw READ getYaw WRITE setYaw)
    void setPitch(float p);
    Q_PROPERTY(float _pitch READ getPitch WRITE setPitch)
    float pitch;
    float yaw;
    bool angle_locked;

    /* Right-clicking allows users to raise a particular Control
     * (to make overlapping work).  This value keeps track of
     * whichever control was raised.
     */
    QPointer<ControlProxy> raised;

    QPointF _click_pos;
    QVector3D _click_pos_world;
    ViewSelector* view_selector;

    QOpenGLShaderProgram height_shader;
    QOpenGLShaderProgram shaded_shader;
    QOpenGLBuffer quad_vertices;

    bool gl_initialized;
};

#endif // VIEWPORT_H
