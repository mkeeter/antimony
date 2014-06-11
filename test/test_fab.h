#ifndef TEST_FAB_H
#define TEST_FAB_H

#include <QObject>

class TestFab : public QObject
{
    Q_OBJECT
public:
    explicit TestFab(QObject *parent = 0);
private slots:
    void ImportFab();
    void ParseExpressions();
    void MoveShape();
    void MoveBounds();
    /*
    void MoveShapeWithBounds();
    void BadTransformExpression();
    */

};

#endif // TEST_TRANSFORM_H
