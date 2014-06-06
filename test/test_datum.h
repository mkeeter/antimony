#ifndef TEST_DATUM_H
#define TEST_DATUM_H

#include <QObject>

class TestDatum : public QObject
{
    Q_OBJECT
private slots:
    void FloatValid();
    void FloatInvalid();
    void FloatValidateFail();
    void FloatValidToInvalid();
    void FloatInvalidToValid();
    void FloatSetSame();
    void SingleInputAccepts();
    void SingleInputLink();
    void SingleInputLinkDelete();
    void SingleInputHasValue();

};

#endif // TEST_DATUM_H
