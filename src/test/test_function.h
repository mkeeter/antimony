#ifndef TEST_FUNCTION_H
#define TEST_FUNCTION_H

#include <QObject>

class NodeRoot;

class TestFunction : public QObject
{
    Q_OBJECT
public:
    TestFunction();

private slots:
    /** Try to make a circle!
     */
    void CircleFunction();

    /** Make a circle with invalid inputs.
     */
    void InvalidCircle();

    /** Change the input to a function datum.
     */
    void ChangeInput();

protected:
    NodeRoot* r;
};

#endif // TEST_FUNCTION_H
