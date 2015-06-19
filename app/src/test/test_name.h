#ifndef TEST_NAME_H
#define TEST_NAME_H

#include <QObject>

class NodeRoot;

class TestName : public QObject
{
    Q_OBJECT
public:
    explicit TestName();

private slots:

    /** Does the node manager return the right name? */
    void NodeName();

    /** Is a particular name valid? */
    void NameValid();

    /** Does the node manager give the right name for multiple nodes? */
    void MultiNodeName();

    /** Does renaming a node work? */
    void Rename();

    /** Does renaming a node with a name that's equivalent post-trimming work? */
    void RenameWithSpaces();

protected:
    NodeRoot* r;
};

#endif // TEST_NAME_H
