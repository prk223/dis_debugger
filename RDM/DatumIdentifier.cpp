#include "DatumIdentifier.h"

DatumIdentifier::DatumIdentifier()
{
    site   = 0;
    app    = 0;
    entity = 0;
}

bool DatumIdentifier::operator==(DatumIdentifier rhs)
{
    bool retVal = false;

    if((site     == rhs.site)   &&
       (app      == rhs.app)    &&
       (entity   == rhs.entity))
    {
        retVal = true;
    }

    return retVal;
}

bool DatumIdentifier::operator!=(DatumIdentifier rhs)
{
    return !(operator==(rhs));
}

void DatumIdentifier::setSite(uint16_t s)
{
    site = s;
}

void DatumIdentifier::setApp(uint16_t a)
{
    app = a;
}

void DatumIdentifier::setEntity(uint16_t e)
{
    entity = e;
}

uint16_t DatumIdentifier::getSite()
{
    return site;
}

uint16_t DatumIdentifier::getApp()
{
    return app;
}

uint16_t DatumIdentifier::getEntity()
{
    return entity;
}
