#ifndef TEST_NAME_H
#define TEST_NAME_H

#include <QObject>

class TestName : public QObject
{
    Q_OBJECT
public:
    explicit TestName(QObject *parent = 0);

private slots:
    void NodeName();
    void NameValid();
    void MultiNodeName();
    void Rename();
    void RenameWithSpaces();

};

#endif // TEST_NAME_H
