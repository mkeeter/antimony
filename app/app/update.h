#pragma once

#include <QNetworkAccessManager>

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    UpdateChecker(QObject* parent);
    void start();
private slots:
    void done(QNetworkReply* reply);
private:
    QNetworkAccessManager* const network;
};
