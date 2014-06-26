/**
 * This plot class is from the Smart-Minotaur project.
 * @author JeGa
 */

#ifndef MOUSE_MONITOR_PLOT_H
#define MOUSE_MONITOR_PLOT_H

#include <memory>

#include <qwt_plot_curve.h>
#include <qwt_plot_layout.h>
#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_plot_marker.h>
#include <QVector>

namespace minotaur
{

    struct CurveContainer {
        QwtPlotCurve curve;
        QVector<double> yData;

        CurveContainer(QString curveTitle, QColor color) : curve(curveTitle) {
            curve.setPen(QPen(color, 1.0));
        }

        ~CurveContainer() {
        }

        void update(double data) {
            yData.append(data);
        }
    };

    enum PLOT_TYPE {LIMITED, NO_LIMIT};

    class MouseMonitorPlot : public QwtPlot
    {
            Q_OBJECT

        private:
            QVector<std::shared_ptr<CurveContainer>> curves;
            QVector<double> xData;

            int xStep;
            int maxSize;

            std::string xAxisTitle;
            std::string yAxisTitle;

            PLOT_TYPE type;

            QwtLegend *legend;
            QwtPlotMarker *marker = nullptr;

        public:
            MouseMonitorPlot(QwtPlot *parent = 0) : QwtPlot(parent) {}
            virtual ~MouseMonitorPlot() {}

            void init(QColor color,
                      std::string title,
                      std::string xAxisTitle,
                      std::string yAxisTitle,
                      std::string curveTitle,
                      PLOT_TYPE type);

            void updatePlot(QVector<double> data);
            void updatePlot(double xIndex, QVector<double> data);

            void clearCurves();
            void clear();

            void setDotStyle();
            void setSticksStyle();

            void addCurve(QString curveTitle, QColor color);

            void addMarker(double xPos, double yPos);

            void setLimit(int limit);
    };

}

#endif
