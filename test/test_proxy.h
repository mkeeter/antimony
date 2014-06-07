#ifndef TEST_PROXY_H
#define TEST_PROXY_H

#include <QObject>

class TestProxy : public QObject
{
    Q_OBJECT
public:
    explicit TestProxy(QObject *parent = 0);
private slots:
    void MakeProxy();
    void GetValidDatum();
    void GetInvalidDatum();
    void GetNonexistentDatum();
    void EvalValid();
};

#endif // TEST_PROXY_H
