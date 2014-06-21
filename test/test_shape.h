#ifndef TEST_SHAPE_H
#define TEST_SHAPE_H

#include <QObject>

class TestShape : public QObject
{
    Q_OBJECT
public:
    explicit TestShape(QObject *parent = 0);

private slots:
    /** Confirm that the default shape is valid (with no inputs)
     */
    void MakeEmptyShape();

    /** Confirm that the shape output datum is behaving as expected.
     */
    void ShapeOutput();

    /** Put an input into a ShapeDatum and make sure it works.
     */
    void ShapeInput();

    /** Put multiple inputs into a ShapeDatum; they should be OR'ed.
     */
    void MultiShapeInput();
};

#endif // TEST_SHAPE_H
