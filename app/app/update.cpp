#include <QRegularExpression>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "app/update.h"

UpdateChecker::UpdateChecker(QObject* parent)
    : QObject(parent), network(new QNetworkAccessManager(this))
{
    connect(network, &QNetworkAccessManager::finished,
                this, &UpdateChecker::done);
}

void UpdateChecker::start()
{
    QNetworkRequest request;
    request.setUrl(QUrl("https://api.github.com/repos/mkeeter/antimony/releases"));
    network->get(request);
}

void UpdateChecker::done(QNetworkReply* reply)
{
    QRegularExpression ver("(\\d+)\\.(\\d+)\\.(\\d+)([a-z]|)");
    if (!ver.match(GITTAG).hasMatch())
    {
        QMessageBox::critical(NULL, "Update error",
                "<b>Update error:</b><br>"
                "Current build is not tagged to any particular version.");
        return;
    }
    auto current = ver.match(GITTAG).capturedTexts();

    if (reply->error() != QNetworkReply::NoError)
    {
        QMessageBox::critical(NULL, "Update error",
                "<b>Update error:</b><br>"
                "Connection failed.");
        return;
    }

    QJsonParseError err;
    auto out = QJsonDocument::fromJson(reply->readAll(), &err);

    if (err.error != QJsonParseError::NoError)
    {
        QMessageBox::critical(NULL, "Update error",
                "<b>Update error:</b><br>"
                "Could not parse JSON file.");
        return;
    }

    auto latest = out.array()[0].toObject();
    auto update = ver.match(latest["tag_name"].toString()).capturedTexts();

    bool available = false;

    // Check for numerical superiority
    for (int i=1; i < 4; ++i)
        if (current[i].toInt() < update[i].toInt())
            available = true;

    // Check for bug-fix release
    if (!update[4].isEmpty() && (current[4].isEmpty() ||
                current[4] < update[4]))
        available = true;

    if (available)
        QMessageBox::information(NULL, "Update available", QString(
                "<b>Update available:</b><br>"
                "This is version %1<br>"
                "Version "
                "<a href=\"https://github.com/mkeeter/antimony/releases/%2\">"
                "%2</a> is available.").arg(current[0])
                                       .arg(update[0]));
    else
        QMessageBox::information(NULL, "No update available",
                "No update is available at this time.");

    deleteLater();
}
