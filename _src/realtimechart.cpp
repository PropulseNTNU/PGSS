#include "realtimechart.h"
#include "globals.h"
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>

RealTimeChart::RealTimeChart(QGraphicsItem* parent, Qt::WindowFlags wFlags) :
    QChart(QChart::ChartTypeCartesian, parent, wFlags), x(0), y(0),
    yRange(globals::CHART_YRANGE), xAxisTitle(""), yAxisTitle(""),
    color(Qt::red), width(1) {

    // Create new series
    newSeries();

    legend()->hide();

}

RealTimeChart::~RealTimeChart() {
    delete series;
    series = nullptr;
}

void RealTimeChart::update(double y) {
    double dx = static_cast<double>(globals::TIMER_UPDATE)/
            static_cast<double>(1000);
    x += dx;
    this->y = y;
    double tick_dx = plotArea().width() / axis->tickCount()*dx;
    if (x > globals::CHART_XRANGE) // Start scrolling when plot is out of range
        scroll(tick_dx,0);

    series->append(x, y);
    if (y > yRange) {
       // yRange = y;
        axisY()->setRange(0, yRange);
    }
}

void RealTimeChart::newSeries() {
    axis = new QValueAxis;
    series = new QLineSeries;

    QPen pen(color);
    pen.setWidth(width);
    series->setPen(pen);
    series->append(x, y);

    addSeries(series);
    createDefaultAxes();
    setAxisX(axis, series);
    axis->setTickCount(globals::CHART_XRANGE);

    axisX()->setRange(0, globals::CHART_XRANGE);
    axisY()->setRange(0, yRange);
    axisX()->setTitleText(xAxisTitle);
    axisY()->setTitleText(yAxisTitle);
}

void RealTimeChart::setXAxisTitle(QString title) {
    xAxisTitle = title;
    axisX()->setTitleText(title);
}

void RealTimeChart::setYAxisTitle(QString title) {
    yAxisTitle = title;
    axisY()->setTitleText(title);
}

void RealTimeChart::setColor(QColor color) {
    this->color = color;
    newSeries();
}

void RealTimeChart::setWidth(int width) {
    this->width = width;
    newSeries();
}
