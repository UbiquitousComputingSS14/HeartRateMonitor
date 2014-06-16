#ifndef HRM_MAIN_WINDOW_H
#define HRM_MAIN_WINDOW_H

#include <QtGui/QMainWindow>
#include <QWidget>
#include <QString>

#include "ui_MainWindow.h"
#include "MouseMonitorPlot.h"
#include "Console.h"
#include "Serial.h"

namespace hrm
{

    class MainWindow : public QMainWindow, public Ui::MainWindow
    {
            Q_OBJECT

        private:
            minotaur::MouseMonitorPlot *plotBroadband;
            minotaur::MouseMonitorPlot *plotIr;
            Console *console;

            Serial *serial;

        private slots:
            void openSerialPortClicked();
            void closeSerialPortClicked();
            void getSettingsClicked();
            void setSampleIntervalClicked();

        public slots:
            void receiveLine(QString line);

        public:
            MainWindow(QWidget *parent = 0);
            virtual ~MainWindow();
    };

}

#endif
