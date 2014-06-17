#include "MouseMonitorPlot.h"

#define DEFAULT_MAX_SIZE 500

namespace minotaur
{

    void MouseMonitorPlot::init(QColor color,
                                std::string title,
                                std::string xAxisTitle,
                                std::string yAxisTitle,
                                PLOT_TYPE type)
    {

        maxSize = DEFAULT_MAX_SIZE;
        xStep = maxSize * 0.10;
        this->type = type;

        curve.setPen(color);
        curve.setSamples(xData, yData);
        curve.attach(this);

        setTitle(QString(title.c_str()));
        setAxisTitle(QwtPlot::xBottom, QString(xAxisTitle.c_str()));
        setAxisTitle(QwtPlot::yLeft, QString(yAxisTitle.c_str()));

        if (type == LIMITED)
            setAxisScale(QwtPlot::xBottom, 0, maxSize, xStep);
    }

    void MouseMonitorPlot::updatePlot(double data)
    {
        xData.append(xData.size());
        yData.append(data);

        curve.setSamples(xData, yData);

        if (type == LIMITED && xData.size() == maxSize) {
            xData.clear();
            yData.clear();
        }

        replot();
    }

    void MouseMonitorPlot::setLimit(int limit)
    {
        maxSize = limit;
        xStep = maxSize * 0.15;

        setAxisScale(QwtPlot::xBottom, 0, maxSize, xStep);
    }

    void MouseMonitorPlot::clear()
    {
        xData.clear();
        yData.clear();

        replot();
    }

}
