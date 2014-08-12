#ifndef TEST_FAB_H
#define TEST_FAB_H

#include <QObject>

class TestFab : public QObject
{
    Q_OBJECT
public:
    explicit TestFab(QObject *parent = 0);
private slots:
    /** Make sure that Python can import the fab module. */
    void ImportFab();

    /** Try parsing a variety of valid and invalid expressions. */
    void ParseExpressions();

    /** Move a shape expression and check that the map is applied. */
    void MoveShape();

    /** Move a bounds object and check that interval arithmetic works. */
    void MoveBounds();

    /** Verify that an invalid shape throws a fab::ParseError. */
    void InvalidShape();

};

#endif // TEST_TRANSFORM_H
