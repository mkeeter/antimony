#include <Python.h>

#include <QApplication>

#include "node/manager.h"
#include "datum/datum.h"

NodeManager* NodeManager::_manager = NULL;

NodeManager::NodeManager(QObject *parent) :
    QObject(parent)
{
    // Nothing to do here
}

QString NodeManager::getName(QString prefix) const
{
    int i = 0;

    bool matched = true;
    QString name;
    while (matched)
    {
        name = prefix + QString::number(i);
        matched = false;
        for (Datum* d : findChildren<Datum*>())
        {
            if (d->objectName() == name)
            {
                i++;
                matched = true;
                break;
            }
        }
    }

    return name;
}

NodeManager* NodeManager::manager()
{
    if (_manager == NULL)
    {
        _manager = new NodeManager(QApplication::instance());
    }
    return _manager;
}
