#ifndef ENTITYDATALIST_H
#define ENTITYDATALIST_H

#include "DatumObserver.h"
#include "DataModelController.h"

#include <string>
#include <map>
#include <QString>
#include <QMutex>
#include <QListWidget>

class EntityDataList : public QListWidget, DatumObserver
{
    Q_OBJECT
protected:
    QMutex mutex;
    // Map entity names to associated datums
    std::map<std::string, std::vector<const DatumInfo*>* > datum_map;
    DataModelController* controller;
    std::string active_entity;

    static QString getDisplayString(const DatumInfo* datum);

public:
    explicit EntityDataList(QWidget *parent = 0);

    void setActiveEntity(std::string entity);

    // DatumObserver inheritance
    virtual void notifyNewDatum(const DatumInfo* datum);
    virtual void notifyNewValue(const DatumInfo* datum);
    virtual void notifyEntityRemoved(std::string entity);

};

#endif // ENTITYDATALIST_H