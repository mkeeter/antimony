#ifndef TEST_PROXY_H
#define TEST_PROXY_H

#include <QObject>

class NodeRoot;

class TestProxy : public QObject
{
    Q_OBJECT
public:
    explicit TestProxy(QObject *parent = 0);
private slots:
    /** Construct a proxy object. */
    void MakeProxy();

    /** Get a value from a proxy using getattr. */
    void GetValidDatum();

    /** Attempt to get a datum that is currently invalid. */
    void GetInvalidDatum();

    /** Attempt to get a nonexistent datum. */
    void GetNonexistentDatum();

    /** Change a datum's name, invalidating downstream datums. */
    void DatumNameChange();

protected:
    NodeRoot* r;
};

#endif // TEST_PROXY_H
