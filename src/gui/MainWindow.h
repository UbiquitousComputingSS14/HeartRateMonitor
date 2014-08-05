#ifndef HRM_MAIN_WINDOW_H
#define HRM_MAIN_WINDOW_H

// TODO: This is not so nice
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
#include "Serial.h"
#include "FFT.h"

namespace hrm
{

    class MainWindow : public QMainWindow, public Ui_MainWindow //Ui::MainWindow
    {
            Q_OBJECT

        private:
            minotaur::MouseMonitorPlot *plotBroadband;
            minotaur::MouseMonitorPlot *plotIr;

            minotaur::MouseMonitorPlot *plotFrequencyIn;
            minotaur::MouseMonitorPlot *plotFrequencyInPaddedData;
            minotaur::MouseMonitorPlot *plotFrequencyOut;
            minotaur::MouseMonitorPlot *plotFrequencyOutComplexData;

            Console *console;
            Serial *serial;

            FFT fft;

            void initPlots();
            void initSignals();
            void displayPeak(int indexMax, double max);
            void displayFrequencies();
            void displayData(SensorData data);

        private slots:
            void openSerialPortClicked();
            void closeSerialPortClicked();
            void getSettingsClicked();
            void setSampleIntervalClicked();

        public slots:
            void receiveLine(QString line);
            void receiveSensorData(SensorData data);
            void receiveSensorSettings(SensorSettings settings);

        public:
            MainWindow(QWidget *parent = 0);
            virtual ~MainWindow();
    };

}

#endif
