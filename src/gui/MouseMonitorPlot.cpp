#include "MouseMonitorPlot.h"

#include <qwt_symbol.h>

#define DEFAULT_MAX_SIZE 300

namespace minotaur
{

    void MouseMonitorPlot::init(QColor color,
                                std::string title,
                                std::string xAxisTitle,
                                std::string yAxisTitle,
                                std::string curveTitle,
                                PLOT_TYPE type)
    {
        maxSize = DEFAULT_MAX_SIZE;
        xStep = maxSize * 0.10;
        this->type = type;

        legend = new QwtLegend(this);
        this->insertLegend(legend);

        auto cContainer = std::make_shared<CurveContainer>(curveTitle.c_str(), color);
        cContainer->curve.setSamples(xData, cContainer->yData);
        cContainer->curve.attach(this);

        curves.append(cContainer);

        setTitle(QString(title.c_str()));
        setAxisTitle(QwtPlot::xBottom, QString(xAxisTitle.c_str()));
        setAxisTitle(QwtPlot::yLeft, QString(yAxisTitle.c_str()));

        setAxisAutoScale(QwtPlot::xBottom);
        setAxisAutoScale(QwtPlot::yLeft);

        if (type == LIMITED)
            setAxisScale(QwtPlot::xBottom, 0, maxSize, xStep);
    }

    void MouseMonitorPlot::updatePlot(QVector<double> data)
    {
        updatePlot(xData.size(), data);
    }

    void MouseMonitorPlot::updatePlot(double xIndex, QVector<double> data)
    {
        if (data.size() != curves.size())
            return;

        xData.append(xIndex);

        for (int i = 0; i < data.size(); ++i) {
            curves[i]->update(data.at(i));
            curves[i]->curve.setSamples(xData, curves[i]->yData);
        }

        if (type == LIMITED && xData.size() == maxSize) {
            clear();
        }

        replot();
    }

    void MouseMonitorPlot::addCurve(QString curveTitle, QColor color)
    {
        auto cContainer = std::make_shared<CurveContainer>(curveTitle, color);
        cContainer->curve.setSamples(xData, cContainer->yData);
        cContainer->curve.attach(this);

        curves.append(cContainer);
    }

    void MouseMonitorPlot::addMarker(double xPos, double yPos)
    {
        QwtSymbol *s = new QwtSymbol(QwtSymbol::RTriangle, Qt::blue, Qt::NoPen, QSize(10, 10));
        marker = new QwtPlotMarker();

        //s->setPinPoint( QPointF( 0.0, 0.0 ) );
        marker->setLabel(QwtText("Peak"));
        marker->setSymbol(s);
        marker->setValue(QPointF(xPos, yPos));
        marker->setLabelAlignment(Qt::AlignRight);
        marker->attach(this);
    }

    void MouseMonitorPlot::setLimit(int limit)
    {
        maxSize = limit;
        xStep = maxSize * 0.15;

        setAxisScale(QwtPlot::xBottom, 0, maxSize, xStep);
    }

    void MouseMonitorPlot::setSticksStyle()
    {
        for (int i = 0; i < curves.size(); ++i) {
            curves[i]->curve.setStyle(QwtPlotCurve::CurveStyle::Sticks);
            curves[i]->curve.setPen(QPen(curves[i]->curve.pen().color(), 1.0));
        }
    }

    void MouseMonitorPlot::setDotStyle()
    {
        for (int i = 0; i < curves.size(); ++i) {
            curves[i]->curve.setStyle(QwtPlotCurve::CurveStyle::Dots);
            curves[i]->curve.setPen(QPen(curves[i]->curve.pen().color(), 3.0));
        }
    }

    void MouseMonitorPlot::clear()
    {
        for (auto i : curves)
            i->yData.clear();
        xData.clear();

        if (marker != nullptr)
            marker->detach();

        replot();
    }

    void MouseMonitorPlot::clearCurves()
    {
        curves.clear();
        xData.clear();

        replot();
    }

}
