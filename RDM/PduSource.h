#ifndef PDUSOURCE_H
#define PDUSOURCE_H

#include <QThread>
#include <QMutex>
#include <time.h>

#include "PduObserver.h"

class PduSource : public QThread
{
private:
    static time_t start_time;

protected:
    uint64_t unknown_pdu_count;
    QMutex observer_mutex;
    std::vector<PduObserver*> observers;
    void notifyObservers(double timestamp, KDIS::KOCTET *raw_data, KDIS::KUINT32 size);

public:
    PduSource();
    virtual ~PduSource();

    void registerPduObserver(PduObserver *obs);
    void removePduObserver(PduObserver *obs);
    virtual void run() = 0;
    static double getTimeSinceStart();
};

#endif // PDUSOURCE_H
