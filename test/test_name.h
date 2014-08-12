#ifndef TEST_NAME_H
#define TEST_NAME_H

#include <QObject>

class TestName : public QObject
{
    Q_OBJECT
public:
    explicit TestName(QObject *parent = 0);

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

};

#endif // TEST_NAME_H
