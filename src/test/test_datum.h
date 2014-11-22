#ifndef TEST_DATUM_H
#define TEST_DATUM_H

#include <QObject>

class TestDatum : public QObject
{
    Q_OBJECT
private slots:

    /** Test that a valid float is labelled as valid. */
    void FloatValid();

    /** Test that a invalid float is labelled as invalid. */
    void FloatInvalid();

    /** Check that a valid expression of the wrong type is invalid. */
    void FloatValidateFail();

    /** Change a float from valid to invalid. */
    void FloatValidToInvalid();

    /** Change a float from invalid to valid. */
    void FloatInvalidToValid();

    /** Set an expression to the same value; no signal should be emitted. */
    void FloatSetSame();

    /** Check single input handler acceptance. */
    void SingleInputAccepts();

    /** Set up a link to a single input handler. */
    void SingleInputLink();

    /** Attempt to make a recursive link. */
    void RecursiveLink();

    /** Test that datums connected through a link are returned.
     */
    void GetInputDatums();

    /** Delete a link to a single input handler. */
    void SingleInputLinkDelete();

    /** Check single input handler value. */
    void SingleInputHasValue();

    /** Check name validation. */
    void NameValidate();

};

#endif // TEST_DATUM_H
