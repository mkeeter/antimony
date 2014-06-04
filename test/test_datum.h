#ifndef TEST_DATUM_H
#define TEST_DATUM_H

#include <QObject>

class TestDatum : public QObject
{
    Q_OBJECT
private slots:
    void testFloatEvalValid();
    void testFloatEvalInvalid();

};

#endif // TEST_DATUM_H
