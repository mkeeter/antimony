#ifndef NODE_H
#define NODE_H

#include <Python.h>

#include <QObject>
#include <QSet>

#include "graph/node/enum.h"

class Control;
class Datum;
class Link;
class Canvas;
class NodeRoot;

class Node : public QObject
{
    Q_OBJECT
public:
    explicit Node(NodeType::NodeType type, NodeRoot* parent);
    explicit Node(NodeType::NodeType type, QString name, NodeRoot* parent);

    /*
     *  On setParent, update NameDatums to check for collisions.
     */
    void setParent(NodeRoot* root);

    /*
     *  Sets the title field of this node
     *  (and may emit titleChanged)
     */
    void setTitle(QString new_title);

    /*
     *  Updates the NameDatum in _name so that it doesn't collide
     *  (used when pasting in nodes)
     */
    void updateName();

    /** Returns a Python proxy that calls getDatum when getattr is called */
    PyObject* proxy();

    /** Looks up a particular Datum by name, return NULL otherwise. */
    Datum* getDatum(QString name) const;

    /** getDatum plus a dynamic cast.
     */
    template <class T>
    T* getDatum(QString name) const
    {
        return dynamic_cast<T*>(getDatum(name));
    }

    /** Returns the NodeType of this node (used in creating Controls)
     */
    NodeType::NodeType getNodeType() const { return type; }

    /** Get this node's name.
     */
    QString getName() const;

    /*
     *  Returns a human-readable title for this node
     *  (e.g. Cube, Sphere, Triangle)
     */
    QString getTitle() const { return title; }

    /*
     *  Returns a set of all the links that connect to this node's datums.
     */
    QSet<Link*> getLinks() const;

signals:
    /*
     *  Emitted when the set of datums stored in this Node changes
     *  (usually through ScriptDatum meta-programming magic).
     */
    void datumsChanged();

    /*
     *  Emitted when this node's title changes.
     *  (usually only happens for ScriptNodes)
     */
    void titleChanged(QString new_title);

protected:
    /** Returns a human-readable name based on type.
     */
    QString getDefaultTitle() const;

    const NodeType::NodeType type;
    Control* control;
    QString title;
};

#endif // NODE_H
