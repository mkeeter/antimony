#ifndef TEST_SHAPE_H
#define TEST_SHAPE_H

#include <QObject>

class TestShape : public QObject
{
    Q_OBJECT
public:
    explicit TestShape(QObject *parent = 0);

private slots:
    void MakeEmptyShape();
};

#endif // TEST_SHAPE_H
