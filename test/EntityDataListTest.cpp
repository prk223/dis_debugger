#include "gtest/gtest.h"
#include <QApplication>

#include "../Visualization/EntityDataList.h"


TEST(EntityDataListTest, RepresentationTest)
{
    // Create QApplication so that we can create QWidgets
    int argc = 0;
    QApplication app(argc, NULL);
    app.closeAllWindows();

    EntityDataList* list = new EntityDataList(NULL);
    list->filterList("THIS IS A FILTER");

    QString rep = list->getStringRepresentation();

    EntityDataList* repList = EntityDataList::createFromStringRepresentation(rep, NULL);

    QString filter    = list->getActiveFilter();
    QString repFilter = repList->getActiveFilter();
    EXPECT_TRUE (filter == repFilter);

    delete list;
    delete repList;
}

