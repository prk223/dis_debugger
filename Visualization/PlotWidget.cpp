#include "PlotWidget.h"
#include "ui_PlotWidget.h"
#include <qwt_plot_layout.h>
#include <qwt_legend.h>
#include <qwt_plot_canvas.h>

const QColor PlotWidget::COLOR_WHEEL[] = {Qt::black,
                                          Qt::red,
                                          Qt::blue,
                                          Qt::green,
                                          Qt::magenta,
                                          Qt::cyan,
                                          Qt::yellow,
                                          Qt::gray,
                                          Qt::darkRed,
                                          Qt::darkBlue,
                                          Qt::darkGreen,
                                          Qt::darkMagenta,
                                          Qt::darkCyan,
                                          Qt::darkYellow,
                                          Qt::darkGray};

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget),
    color_index(0),
    delete_always_hidden(false),
    hidden(false)
{
    ui->setupUi(this);
    QString toolTip = "Select area to zoom\n";
    toolTip += "Press 'U' to undo zoom\n";
    toolTip += "Press 'R' to redo zoom\n";
    toolTip += "Press HOME key to return to autoscale";
    setToolTip(toolTip);

    // Put a legend on plot
    QwtLegend* legend = new QwtLegend();
    legend->setFrameStyle(QFrame::Box | QFrame::Sunken);
    ui->embedded_plot->insertLegend(legend, QwtPlot::BottomLegend);

    zoomer = new QwtPlotZoomer(ui->embedded_plot->canvas());
    curves.clear();

    // Setup zoomer
    zoomer->setMousePattern(QwtEventPattern::MouseSelect1, Qt::LeftButton);
    zoomer->setKeyPattern(QwtEventPattern::KeyRedo, Qt::Key_R);
    zoomer->setKeyPattern(QwtEventPattern::KeyUndo, Qt::Key_U);
    zoomer->setKeyPattern(QwtEventPattern::KeyHome, Qt::Key_Home );
    connect(zoomer, SIGNAL(zoomed(QRectF)), this, SLOT(zoomChanged(QRectF)));

    min_plot_size   = ui->embedded_plot->minimumSize();
    max_height      = maximumHeight();
    normal_size     = size();
    min_size        = minimumSize();

    focused_style   = styleSheet();
    unfocused_style = focused_style;
    unfocused_style.replace("solid", "dotted");
    setStyleSheet(unfocused_style);
    QApplication* app = qApp;
    connect(app, SIGNAL(focusChanged(QWidget*,QWidget*)),
            this, SLOT(focusChange(QWidget*, QWidget*)));
}

PlotWidget::~PlotWidget()
{
    clearCurves();
    delete zoomer;
    delete ui;
}

QColor PlotWidget::getNextColor()
{
    QColor retVal = COLOR_WHEEL[color_index];
    color_index++;
    size_t numColors = sizeof(COLOR_WHEEL) / sizeof(COLOR_WHEEL[0]);
    if(color_index >= numColors)
        color_index = 0;
    return retVal;
}

void PlotWidget::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PlotWidget::zoomChanged(const QRectF&)
{
    const QwtPlotZoomer* zoomer = dynamic_cast<const QwtPlotZoomer*>(sender());
    if(zoomer != NULL)
    {
        // If stack index is 0, then we cannot undo any more, so we're at the
        // initial state when autoscale should be on
        if(zoomer->zoomRectIndex() == 0)
        {
            ui->embedded_plot->setAxisAutoScale(zoomer->xAxis(), true);
            ui->embedded_plot->setAxisAutoScale(zoomer->yAxis(), true);
            ui->embedded_plot->replot();
        }
    }
}

void PlotWidget::focusChange(QWidget* oldWidget, QWidget* newWidget)
{
    // When the plot's canvas takes focus, you are able to undo/redo
    // Change widget's border so user knows what plot they can alter
    QwtPlotCanvas* canvas = dynamic_cast<QwtPlotCanvas*>
            (ui->embedded_plot->canvas());
    if((oldWidget == canvas) && (newWidget != canvas))
    {
        setStyleSheet(unfocused_style);
    }
    else if((oldWidget != canvas) && (newWidget == canvas))
    {
        setStyleSheet(focused_style);
    }
}

void PlotWidget::on_ClearBtn_clicked()
{
   clearCurves();
}

void PlotWidget::on_DeleteBtn_clicked()
{
    emit deletePlot(this);
}

void PlotWidget::on_HideBtn_clicked()
{
    if(hidden) // unhide
    {
        ui->embedded_plot->setMinimumSize(min_plot_size);
        setMaximumHeight(max_height);
        setMinimumSize(normal_size); // force normal size
        setMinimumSize(min_size);    // allow user to shrink
        ui->embedded_plot->show();
        ui->ClearBtn->show();
        ui->ResetBtn->show();
        ui->FullHistoryBtn->show();
        if(!delete_always_hidden) ui->DeleteBtn->show();
        ui->HideBtn->setText("Hide");
        ui->HideBtn->setToolTip("Hide plot");
    }
    else // hide
    {
        // Save unhidden sizes prior to hiding
        min_plot_size   = ui->embedded_plot->minimumSize();
        max_height      = maximumHeight();
        normal_size     = size();
        min_size        = minimumSize();
        ui->embedded_plot->setMinimumSize(0, 0);
        setMinimumHeight(0);
        setMaximumHeight(ui->ClearBtn->height() + 15);
        ui->embedded_plot->hide();
        ui->ClearBtn->hide();
        ui->ResetBtn->hide();
        ui->FullHistoryBtn->hide();
        ui->DeleteBtn->hide();
        ui->HideBtn->setText("Show");
        ui->HideBtn->setToolTip("Show plot");
    }

    hidden = !hidden;
}

void PlotWidget::on_FullHistoryBtn_clicked()
{
    for(int curveIdx = 0; curveIdx < curves.size(); curveIdx++)
    {
        PlotCurveItem* curve = curves.at(curveIdx);
        curve->showFullHistory();
    }
}

void PlotWidget::on_ResetBtn_clicked()
{
    for(int curveIdx = 0; curveIdx < curves.size(); curveIdx++)
    {
        PlotCurveItem* curve = curves.at(curveIdx);
        curve->truncateHistory();
    }
}

void PlotWidget::hideDelete()
{
    delete_always_hidden = true;
    ui->DeleteBtn->hide();
}

void PlotWidget::addCurve(EntityDatumItem* item)
{
    if(item != NULL)
    {
        const DatumInfo* datum = item->getWatchedDatum();
        if(datum != NULL)
        {
            PlotCurveItem* curve = new PlotCurveItem(ui->embedded_plot,
                                                     datum,
                                                     getNextColor());
            curves.append(curve);
            curve->activate(this);
        }

        // Also add any children
        for(int kidIdx = 0; kidIdx < item->childCount(); kidIdx++)
        {
            EntityDatumItem* kid =
                    dynamic_cast<EntityDatumItem*>(item->child(kidIdx));
            addCurve(kid);
        }
    }
}

void PlotWidget::addCurves(QList<QTreeWidgetItem*> items)
{
    for(int itemNum = 0; itemNum < items.size(); itemNum++)
    {
        EntityDatumItem* item =
                dynamic_cast<EntityDatumItem*>(items.at(itemNum));
        addCurve(item);
    }
}

void PlotWidget::clearCurves()
{
    while(curves.size())
    {
        PlotCurveItem* i = curves.at(0);
        curves.removeAt(0);
        i->detach(); // Remove from plot
        delete i;
    }
    ui->embedded_plot->replot();
}

bool PlotWidget::equivalentTo(PlotWidget* rhs)
{
    bool ret = true;
    if(color_index != rhs->color_index)
        ret = false;
    else if(delete_always_hidden != rhs->delete_always_hidden)
        ret = false;
    else if(hidden != rhs->hidden)
        ret = false;
    else if(curves.length() != rhs->curves.length())
        ret = false;
    else
    {
        for(int idx = 0; idx < curves.length(); idx++)
        {
            if(!(curves.at(idx)->equivalentTo(rhs->curves.at(idx))))
            {
                ret = false;
                break;
            }
        }
    }

    return ret;
}

QString PlotWidget::getStringRepresentation() const
{
    // Get current sizes
    // If hidden, use last saved sizes
    QSize minPlotSize  = min_plot_size;
    int   maxHeight    = max_height;
    QSize normalSize   = normal_size;
    QSize minSize      = min_size;
    if(!hidden)
    {   // If not hidden, get most up to date values
        minPlotSize  = ui->embedded_plot->minimumSize();
        maxHeight    = maximumHeight();
        normalSize   = size();
        minSize      = minimumSize();
    }

    QString rep("<PlotWidget>\n");
    rep += "<LastColor>"    + QString::number(color_index)            + "</LastColor>\n";
    rep += "<DeleteHidden>" + QString::number(delete_always_hidden)   + "</DeleteHidden>\n";
    rep += "<Hidden>"       + QString::number(hidden)                 + "</Hidden>\n";
    rep += "<MinPlotSizeW>" + QString::number(minPlotSize.width())    + "</MinPlotSizeW>\n";
    rep += "<MinPlotSizeH>" + QString::number(minPlotSize.height())   + "</MinPlotSizeH>\n";
    rep += "<MaxHeight>"    + QString::number(maxHeight)              + "</MaxHeight>\n";
    rep += "<NormalSizeW>"  + QString::number(normalSize.width())     + "</NormalSizeW>\n";
    rep += "<NormalSizeH>"  + QString::number(normalSize.height())    + "</NormalSizeH>\n";
    rep += "<MinSizeW>"     + QString::number(minSize.width())        + "</MinSizeW>\n";
    rep += "<MinSizeH>"     + QString::number(minSize.height())       + "</MinSizeH>\n";
    for(int idx = 0; idx < curves.length(); idx++)
        rep += "<Curve>" + curves.at(idx)->getStringRepresentation()  + "</Curve>\n";
    rep += "</PlotWidget>\n";

    return rep;
}

void PlotWidget::setFromStringRepresentation(QString rep)
{
    // Clear any curves that were already on the plot
    clearCurves();

    QString guts = QString(Configuration::getTagValue(rep.toStdString(), "PlotWidget").c_str());
    color_index = QString(Configuration::getTagValue(guts.toStdString(), "LastColor").c_str()).toInt();

    int w = QString(Configuration::getTagValue(guts.toStdString(),
                                             "MinPlotSizeW").c_str()).toInt();
    int h = QString(Configuration::getTagValue(guts.toStdString(),
                                             "MinPlotSizeH").c_str()).toInt();
    min_plot_size = QSize(w, h);
    max_height = QString(Configuration::getTagValue(guts.toStdString(),
                                             "MaxHeight").c_str()).toInt();
    w = QString(Configuration::getTagValue(guts.toStdString(),
                                             "NormalSizeW").c_str()).toInt();
    h = QString(Configuration::getTagValue(guts.toStdString(),
                                             "NormalSizeH").c_str()).toInt();
    normal_size = QSize(w, h);
    w = QString(Configuration::getTagValue(guts.toStdString(),
                                             "MinSizeW").c_str()).toInt();
    h = QString(Configuration::getTagValue(guts.toStdString(),
                                             "MinSizeH").c_str()).toInt();
    min_size = QSize(w, h);
    ui->embedded_plot->setMinimumSize(min_size);
    setMaximumHeight(max_height);
    setMinimumSize(normal_size); // force normal size
    setMinimumSize(min_size); // allow user to shrink if desired

    bool cannotDelete = QString(Configuration::getTagValue(guts.toStdString(), "DeleteHidden").c_str()).toInt() != 0;
    if(cannotDelete) hideDelete();
    bool hide = QString(Configuration::getTagValue(guts.toStdString(), "Hidden").c_str()).toInt() != 0;
    if(hide) QTimer::singleShot(500, this, SLOT(on_HideBtn_clicked()));

    // Get all curves
    QString endTag = "</Curve>";
    QString curveData = QString(Configuration::getTagValue(guts.toStdString(),
                                               "Curve").c_str());
    while(curveData != "")
    {
        PlotCurveItem* curveItem = PlotCurveItem::
                createFromStringRepresentation(curveData, ui->embedded_plot);
        curves.append(curveItem);
        curveItem->activate(this);

        int endTagPos = guts.indexOf(endTag);
        if(endTagPos >= 0)
        {
            guts = guts.remove(0, endTagPos + endTag.length());
            curveData = QString(Configuration::getTagValue(guts.toStdString(),
                                                           "Curve").c_str());
        }
        else
        {
            std::cerr << __FILE__ << ": Broken XML:" + rep.toStdString() << std::endl;
            break;
        }
    }
}

PlotWidget* PlotWidget::createFromStringRepresentation(QString rep,
                                                    QWidget* parent)
{
    PlotWidget* ret = new PlotWidget(parent);
    ret->setFromStringRepresentation(rep);
    return ret;
}
