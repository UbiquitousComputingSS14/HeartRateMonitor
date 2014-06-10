#ifndef HRM_MAIN_WINDOW_H
#define HRM_MAIN_WINDOW_H

#include <QtGui/QMainWindow>
#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>

#include "ui_hrmMainWindow.h"
#include "MouseMonitorPlot.h"
#include "hrmConsole.h"

namespace hrm
{

    struct serialPortSettings {
        QString portName;
        int baudrate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
    };

    class hrmMainWindow : public QMainWindow, public Ui::hrmMainWindow
    {
            Q_OBJECT

        private:
            minotaur::MouseMonitorPlot *plotBroadband;
            minotaur::MouseMonitorPlot *plotIr;

            QSerialPort *serial;
            serialPortSettings settings;

            hrmConsole *console;

            void writeData(const QByteArray &data);

        private slots:
            void readData();
            void handleError(QSerialPort::SerialPortError error);
            void openSerialPort();
            void closeSerialPort();
            void getSettings();

        public:
            hrmMainWindow(QWidget *parent = 0);
            virtual ~hrmMainWindow();
    };

}

#endif
