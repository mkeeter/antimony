#ifndef TEST_NODE_H
#define TEST_NODE_H

#include <QObject>

class TestNode : public QObject
{
    Q_OBJECT
public:
    explicit TestNode(QObject *parent = 0);

private slots:
    void GetDatum();
    void DeleteNode();

};

#endif // TEST_NODE_H
