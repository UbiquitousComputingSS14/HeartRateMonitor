#include "hrmMainWindow.h"

#include <QMessageBox>
#include <QList>

namespace hrm
{

    hrmMainWindow::hrmMainWindow(QWidget *parent) : QMainWindow(parent)
    {
        setupUi(this);

        plotBroadband = new minotaur::MouseMonitorPlot();
        plotBroadband->init(Qt::blue, "Broadband", "", "");
        graphLayout->layout()->addWidget(plotBroadband);

        plotIr = new minotaur::MouseMonitorPlot();
        plotIr->init(Qt::red, "IR", "", "");
        graphLayout->layout()->addWidget(plotIr);

        serial = new QSerialPort(this);

        connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
        connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
                SLOT(handleError(QSerialPort::SerialPortError)));

        // Init default settings
        settings.portName = "/dev/ttyACM0";
        settings.baudrate = 9600;
        settings.dataBits = QSerialPort::Data8;
        settings.parity = QSerialPort::NoParity;
        settings.stopBits = QSerialPort::OneStop;
        settings.flowControl = QSerialPort::NoFlowControl;

        openSerialPort();
    }

    hrmMainWindow::~hrmMainWindow()
    {
        delete plotBroadband;
        delete plotIr;
    }

    void hrmMainWindow::writeData(const QByteArray &data)
    {
        serial->write(data);
    }

    void hrmMainWindow::readData()
    {
        if (serial->canReadLine()) {
            QByteArray data = serial->readLine();
            QList<QByteArray> dataWords = data.split(' ');

            console->append(data.trimmed());

            if (dataWords.size() == 4) {
                if (dataWords[0] == "broadband" && dataWords[2] == "ir") {
                    plotBroadband->updatePlot(dataWords[1].toDouble());

                    plotIr->updatePlot(dataWords[3].trimmed().toDouble());
                }
            }
        }
    }

    void hrmMainWindow::openSerialPort()
    {
        serial->setPortName(settings.portName);
        serial->setBaudRate(settings.baudrate);
        serial->setDataBits(settings.dataBits);
        serial->setParity(settings.parity);
        serial->setStopBits(settings.stopBits);
        serial->setFlowControl(settings.flowControl);

        if (serial->open(QIODevice::ReadWrite)) {
            //QMessageBox::information(this, tr("Info"), tr("Opened serial port."));

            console->append("Opened serial port\n");
        } else {
            QMessageBox::critical(this, tr("Error"), serial->errorString());
        }
    }

    void hrmMainWindow::closeSerialPort()
    {
        serial->close();
    }

    void hrmMainWindow::handleError(QSerialPort::SerialPortError error)
    {
        if (error == QSerialPort::ResourceError) {
            QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
            closeSerialPort();
        }
    }

}
