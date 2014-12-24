#ifndef TEST_NODE_H
#define TEST_NODE_H

#include <QObject>

class NodeRoot;

class TestNode : public QObject
{
    Q_OBJECT
public:
    explicit TestNode();

private slots:
    /** Can you get a datum from a node? */
    void GetDatum();

    /** Does a node emit destroyed when deleted? */
    void DeleteNode();

    /** Test evaluating interlinked nodes. */
    void EvalValid();

    /** Change one node's name; make another node valid. */
    void NameChangeEval();

    /** Create a new node, making another node valid. */
    void NewNodeCreation();

    /** Assign p.x = p.x and make sure it fails.
     */
    void DirectRecursiveConnection();

    /** Assign p.x = p.y, p.y = p.x and make sure it fails.
     */
    void LoopingRecursiveConnection();

    /** More complicated recursive connection.
     */
    void ComplexRecursiveConnection();

    /** If a recursive connection is fixed, all nodes should become valid
     */
    void ModifyRecursiveConnection();

    /** Creates a node with child nodes and verifies that it works.
     */
    void TestChildNodes();

protected:
    NodeRoot* r;
};

#endif // TEST_NODE_H
