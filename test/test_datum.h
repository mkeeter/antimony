#ifndef TEST_DATUM_H
#define TEST_DATUM_H

#include <QObject>

class TestDatum : public QObject
{
    Q_OBJECT
private slots:
    void testFloatValid();
    void testFloatInvalid();
    void testFloatValidToInvalid();
    void testFloatInvalidToValid();

};

#endif // TEST_DATUM_H
