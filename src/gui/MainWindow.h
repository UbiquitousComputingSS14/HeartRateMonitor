#ifndef HRM_MAIN_WINDOW_H
#define HRM_MAIN_WINDOW_H

#ifdef Qt5
#include <QMainWindow>
#else
#include <QtGui/QMainWindow>
#endif

#include <QWidget>
#include <QString>

#include "ui_MainWindow.h"

#include "MouseMonitorPlot.h"
#include "Console.h"
#include "FFT.h"
#include "Controller.h"

namespace hrm
{

    class MainWindow : public QMainWindow, public Ui_MainWindow
    {
            Q_OBJECT

        private:
            Controller controller;

            minotaur::MouseMonitorPlot *plotBroadband;
            minotaur::MouseMonitorPlot *plotIr;

            minotaur::MouseMonitorPlot *plotFrequencyIn;
            minotaur::MouseMonitorPlot *plotFrequencyInPaddedData;
            minotaur::MouseMonitorPlot *plotFrequencyOut;
            minotaur::MouseMonitorPlot *plotFrequencyOutComplexData;

            Console *console;

            void initPlots();
            void initSignals();

            void displayPeak(int indexMax, double max);

        private slots:
            void openSerialPortClicked();
            void closeSerialPortClicked();
            void getSettingsClicked();
            void setSampleIntervalClicked();

            void sensorSettings(
                SensorSettings settings,
                FFT_properties properties);
            void sensorData(SensorData data);
            void frequencySpectrum(
                std::vector<double>& magnitude,
                int peakIndex);

        public:
            MainWindow(QWidget *parent = 0);
            virtual ~MainWindow();
    };

}

#endif
