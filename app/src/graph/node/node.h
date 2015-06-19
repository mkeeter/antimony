#ifndef NODE_H
#define NODE_H

#include <Python.h>

#include <QObject>
#include <QSet>

class Datum;
class Link;
class Canvas;
class NodeRoot;

class Node : public QObject
{
    Q_OBJECT
public:
    explicit Node(NodeRoot* parent);
    explicit Node(QString name, NodeRoot* parent);

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
     *  Updates the NameDatum in __name so that it doesn't collide
     *  (used when pasting in nodes)
     */
    void updateName();

    /*
     *  Returns a Python proxy that calls getDatum when getattr is called.
     *
     *  If caller is given, calls setUpstream during looking.
     *  If settable is true, setattr can be called on the proxy object.
     */
    PyObject* proxy(Datum* caller=NULL, bool settable=false);
    PyObject* mutableProxy();

    /** Looks up a particular Datum by name, return NULL otherwise. */
    Datum* getDatum(QString name) const;

    /** getDatum plus a dynamic cast.
     */
    template <class T>
    T* getDatum(QString name) const
    {
        return dynamic_cast<T*>(getDatum(name));
    }

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
     *  Emitted when the order of datums stored in this node has changed.
     *  (usually through ScriptDatum meta-programming magic).
     */
    void datumOrderChanged();

    /*
     *  Emitted when this node's title changes.
     *  (usually only happens for ScriptNodes)
     */
    void titleChanged(QString new_title);

    /*
     *  Emitted before evaluating a script that modifies Control objects.
     *
     *  Sets the touched flag to false so that afterwards Controls can
     *  see if they've been touched or not (and delete themselves).
     */
    void clearControlTouchedFlag();

    /*
     *  Used to request that Controls with touched == false delete themselves.
     */
    void deleteUntouchedControls();

protected:
    QString title;
};


Node* ScriptNode(QString name, QString script, NodeRoot* parent);
Node* ScriptNode(QString script, NodeRoot* parent);
Node* ScriptNode(NodeRoot* parent);

#endif // NODE_H
