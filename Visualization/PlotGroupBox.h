#ifndef PLOTGROUPBOX_H
#define PLOTGROUPBOX_H

#include "DatumInfo.h"
#include "PlotWidget.h"

#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLabel>
#include <vector>

namespace Ui {
class PlotGroupBox;
}

class PlotGroupBox : public QWidget
{
    Q_OBJECT

private:
    Ui::PlotGroupBox* ui;
    QList<PlotWidget*> created_plots;

protected:
    virtual void dragEnterEvent(QDragEnterEvent* event);
    virtual void dropEvent(QDropEvent* event);

public:
    explicit PlotGroupBox(QWidget* parent = 0);
    virtual ~PlotGroupBox();
};

#endif // PLOTGROUPBOX_H
