#ifndef REALTIMECHART_H
#define REALTIMECHART_H

#include <QtCharts>
#include "globals.h"
// See https://doc.qt.io/qt-5/qtcharts-dynamicspline-example.html

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QValueAxis;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class RealTimeChart : public QChart
{
public:
    RealTimeChart(int yRange = globals::CHART_YRANGE, int yRangeNeg = 0,
                  QGraphicsItem* parent = 0, Qt::WindowFlags wFlags = 0);
    ~RealTimeChart();

public slots:
    void setXAxisTitle(QString title);
    void setYAxisTitle(QString title);
    void setColor(QColor color);
    void setWidth(int width);

    void update(double y);
    void reset();
    void newSeries();


private:
    QLineSeries* series;
    QStringList titles;
    QValueAxis* axis;
    QString xAxisTitle;
    QString yAxisTitle;
    QColor color;
    int width;
    double x;
    double y;
    int yRange;
    int yRangeNeg;
};

#endif // REALTIMECHART_H
