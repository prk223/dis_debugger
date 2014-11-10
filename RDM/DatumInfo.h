#ifndef DATUMINFO_H
#define DATUMINFO_H

#include "DatumValue.h"
#include "DatumIdentifier.h"

#include <QMutex>
#include <vector>
#include <map>
#include <string>

class DatumInfo
{
private:
    QMutex *mutex;
    DatumIdentifier identifier;
    std::string type;
    std::string unit;
    std::string unit_class;
    std::string name;
    std::string category;
    std::string description;
    std::vector<DatumValue*> values;

    DatumInfo();

protected:
    DatumValue* getRawDatumValue();

public:
    ~DatumInfo();
    //DatumInfo & operator=(const DatumInfo& copyVal);
    //DatumInfo(const DatumInfo& copyVal);

    static DatumInfo* createDatum(std::string type, QByteArray value);
    double getLastTimestamp();
    QByteArray getLastRawValue();

    bool hasSameId(DatumInfo* rhs);
    void setId(DatumIdentifier id);
    void setUnit(std::string u);
    void setUnitClass(std::string u);
    void setName(std::string n);
    void setCategory(std::string c);
    void setDescription(std::string d);
    DatumIdentifier getId() const;
    std::string getEntityName() const;
    std::string getType() const;
    std::string getUnit() const;
    std::string getUnitClass() const;
    std::string getName() const;
    std::string getCategory() const;
    std::string getDescription() const;
    std::string getValue();
    std::map<double, std::string> getHistory() const;
    bool addValue(double time, QByteArray value);
    void truncateHistory(double currentTime);
};

#endif // DATUMINFO_H
