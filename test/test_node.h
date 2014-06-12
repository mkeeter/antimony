#ifndef TEST_NODE_H
#define TEST_NODE_H

#include <QObject>

class TestNode : public QObject
{
    Q_OBJECT
public:
    explicit TestNode(QObject *parent = 0);

private slots:
    /** Can you get a datum from a node? */
    void GetDatum();

    /** Does a node emit destroyed when deleted? */
    void DeleteNode();

    /** Test evaluating interlinked nodes. */
    void EvalValid();

    /** Change one node's name; make another node valid. */
    void NameChangeEval();

};

#endif // TEST_NODE_H
