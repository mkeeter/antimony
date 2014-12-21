#ifndef MANAGER_H
#define MANAGER_H

#include <Python.h>
#include <QObject>

#include "fab/types/shape.h"

class NameDatum;
class Datum;
class Canvas;
class Node;
class Control;

class NodeManager : public QObject
{
    Q_OBJECT
public:
    explicit NodeManager(QObject *parent = 0);

    /** Returns a Python object suitable for use as a globals dictionary.
     *
     *  This dictionary contains builtins and nodes indexing to proxies,
     *  as well as the built-in math module.
     */
    PyObject* proxyDict(Datum* caller);

    /** Gets an unused name for the given prefix.
     */
    QString getName(QString prefix) const;

    /** Returns the singleton manager object.
     */
    static NodeManager* manager();

    /** Checks to see if the given name is unique.
     */
    bool isNameUnique(QString name) const;

    /** Returns a NameDatum with matching name, or NULL.
     */
    NameDatum* findMatchingName(PyObject* proposed) const;

    /** Deletes all nodes.
     */
    void clear();

    /** Deserializes the given scene.
     *  Returns true on success, false otherwise.
     */
    bool deserializeScene(QByteArray in);

    /** Returns the union of all unconnected shapes in the scene.
     */
    Shape getCombinedShape();

    /** Returns a map of name -> shape for all unconnected shapes.
     */
    QMap<QString, Shape> getShapes();

#ifdef ANTIMONY
    /** Creates controls for top-level nodes.
     */
    void makeControls(Canvas* canvas);

    /** Creates connections (UI elements representing links).
     */
    void makeConnections(Canvas* canvas);
#endif

public slots:
    /** Triggers an update on all EvalDatums that use the new name.
     */
    void onNameChange(QString new_name);

protected:
    static NodeManager* _manager;
};

#endif // MANAGER_H
