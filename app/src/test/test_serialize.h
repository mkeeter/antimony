#ifndef TEST_SERIALIZE_H
#define TEST_SERIALIZE_H

#include <QObject>

class TestSerialize : public QObject
{
    Q_OBJECT
private slots:
    /** Serialize a scene with one object then deserialize it.
     */
    void SerializeSimpleScene();
};

#endif
