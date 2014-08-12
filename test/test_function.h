#ifndef TEST_FUNCTION_H
#define TEST_FUNCTION_H

#include <QObject>

class TestFunction : public QObject
{
    Q_OBJECT
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
};

#endif // TEST_FUNCTION_H
