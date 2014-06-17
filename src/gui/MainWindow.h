#ifndef HRM_MAIN_WINDOW_H
#define HRM_MAIN_WINDOW_H

#include <QtGui/QMainWindow>
#include <QWidget>
#include <QString>
#include <QQueue>

#include "ui_MainWindow.h"
#include "MouseMonitorPlot.h"
#include "Console.h"
#include "Serial.h"
#include "FFT.h"

namespace hrm
{

    class MainWindow : public QMainWindow, public Ui::MainWindow
    {
            Q_OBJECT

        private:
            minotaur::MouseMonitorPlot *plotBroadband;
            minotaur::MouseMonitorPlot *plotIr;
            minotaur::MouseMonitorPlot *plotFrequencyIn;
            minotaur::MouseMonitorPlot *plotFrequencyOut;
            Console *console;

            Serial *serial;

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
