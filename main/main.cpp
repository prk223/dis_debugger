#include <QThread>
#include <iostream>
#include <time.h>

#include "DataModelController.h"
#include "DatumObserver.h"

// KDIS includes
#include "NetworkPduSource.h"
#include "KDIS/Network/Connection.h" // A cross platform connection class.

using namespace std;
using namespace KDIS;
using namespace DATA_TYPE;
using namespace ENUMS;
using namespace UTILS;
using namespace NETWORK;

class MainObs : public DatumObserver
{
private:
    DataModelController* controller;
    std::map<std::string, std::map<std::string, std::vector<DatumInfo*> > > datums;

public:
    MainObs(DataModelController* cont){controller = cont;}
    virtual ~MainObs(){}
    virtual void notifyNewDatum(DatumInfo* datum)
    {
        if(datum->getId().getEntity() == 0)
            return;
        if(datums.size() > 0)
        {
            std::map<std::string, std::map<std::string, std::vector<DatumInfo*> > >::iterator it;
            it = datums.begin();
            if(it->first != datum->getEntityName())
                return;
        }

        controller->registerDatumObserver(this, datum);
        bool foundEntity = false;
        std::map<std::string, std::map<std::string, std::vector<DatumInfo*> > >::iterator it;
        for(it = datums.begin(); it != datums.end(); it++)
        {
            if(it->first == datum->getEntityName())
            {
                foundEntity = true;
                bool foundCat = false;
                std::map<std::string, std::vector<DatumInfo*> >::iterator itInner;
                for(itInner = it->second.begin(); itInner != it->second.end(); itInner++)
                {
                    if(itInner->first == datum->getCategory())
                    {
                        foundCat = true;
                        itInner->second.push_back(datum);
                    }
                }
                if(!foundCat)
                {
                    std::vector<DatumInfo*> catList;
                    catList.push_back(datum);
                    it->second[datum->getCategory()] = catList;
                }
            }
        }
        if(!foundEntity)
        {
            std::vector<DatumInfo*> catList;
            catList.push_back(datum);
            std::map<std::string, std::vector<DatumInfo*> > catDatums;
            catDatums[datum->getCategory()] = catList;
            datums[datum->getEntityName()] = catDatums;
        }
        redraw();
    }

    virtual void notifyNewValue(DatumInfo* datum)
    {
        redraw();
    }

    virtual void notifyEntityRemoved(std::string entity)
    {
        if(datums.count(entity))
            datums.erase(entity);
        redraw();
    }

    void redraw()
    {
        // Clear screen
        std::cout << std::endl << std::endl << std::endl << std::endl;
        std::cout << std::endl << std::endl << std::endl << std::endl;
        std::cout << std::endl << std::endl << std::endl << std::endl;
        std::cout << std::endl << std::endl << std::endl << std::endl;
        std::cout << std::endl << std::endl << std::endl << std::endl;
        std::cout << std::endl << std::endl << std::endl << std::endl;
        std::cout << std::endl << std::endl << std::endl << std::endl;
        std::cout << std::endl << std::endl << std::endl << std::endl;

        // Print current datum values
        std::map<std::string, std::map<std::string, std::vector<DatumInfo*> > >::iterator it;
        for(it = datums.begin(); it != datums.end(); it++)
        {
           std::cout << it->first << std::endl;
           std::map<std::string, std::vector<DatumInfo*> >::iterator itCat;
           for(itCat = it->second.begin(); itCat != it->second.end(); itCat++)
           {
               std::cout << "  " << itCat->first << std::endl;
               std::vector<DatumInfo*>::iterator itDatum;
               for(itDatum = itCat->second.begin(); itDatum != itCat->second.end(); itDatum++)
               {
                   std::cout << "    " << (*itDatum)->getName();
                   std::cout << ": "   << (*itDatum)->getValue() << std::endl;
               }
           }
        }
    }
};


int main()
{
    DataModelController *controller = new DataModelController();
    MainObs* obs = new MainObs(controller);
    controller->registerObserver(obs);
    controller->loadMetadataXml("C:\\Comp\\school_work\\CSE\\dis_debugger\\dis_definitions.xml");

    while(true)
        QThread::msleep(1000);

    return 0;
}
