#include "hrmMainWindow.h"

#include <QMessageBox>
#include <QList>

namespace hrm
{

    hrmMainWindow::hrmMainWindow(QWidget *parent) : QMainWindow(parent)
    {
        setupUi(this);

        plotBroadband = new minotaur::MouseMonitorPlot();
        plotBroadband->init(Qt::blue, "Broadband", "Sample", "Sensor Data");
        graphLayout->layout()->addWidget(plotBroadband);

        plotIr = new minotaur::MouseMonitorPlot();
        plotIr->init(Qt::red, "IR", "Sample", "Sensor Data");
        graphLayout->layout()->addWidget(plotIr);

        console = new hrmConsole();
        mainLayout->layout()->addWidget(console);

        serial = new QSerialPort(this);

        connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
        connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
                SLOT(handleError(QSerialPort::SerialPortError)));

        actionConnect->setEnabled(true);
        actionDisconnect->setEnabled(false);

        connect(actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPort()));
        connect(actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPort()));

        statusbar->showMessage(tr("Disconnected"));

        connect(getSettingsBtn, SIGNAL(clicked()), this, SLOT(getSettings()));

        // Init default settings
        settings.portName = "/dev/ttyACM0";
        settings.baudrate = 9600;
        settings.dataBits = QSerialPort::Data8;
        settings.parity = QSerialPort::NoParity;
        settings.stopBits = QSerialPort::OneStop;
        settings.flowControl = QSerialPort::NoFlowControl;
    }

    hrmMainWindow::~hrmMainWindow()
    {
        closeSerialPort();

        delete plotBroadband;
        delete plotIr;
        delete serial;
        delete console;
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

            console->print(data);

            data = data.trimmed();

            if (dataWords.size() >= 1) {
                if (dataWords[0] == "data:") {
                    if (dataWords.size() != 5)
                        return;

                    if (dataWords[1] == "broadband" && dataWords[3] == "ir") {
                        plotBroadband->updatePlot(dataWords[2].toDouble());
                        plotIr->updatePlot(dataWords[4].trimmed().toDouble());
                    }
                } else if (dataWords[0] == "settings:") {
                    if (dataWords.size() != 11)
                        return;

                    sensorEdit->setText(dataWords[2]);
                    idEdit->setText(dataWords[4]);
                    maxValEdit->setText(dataWords[6]);
                    minValEdit->setText(dataWords[8]);
                    resolutionEdit->setText(dataWords[10]);
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
            statusbar->showMessage(tr("Connected"));

            actionConnect->setEnabled(false);
            actionDisconnect->setEnabled(true);
        } else {
            QMessageBox::critical(this, tr("Error"), serial->errorString());
        }
    }

    void hrmMainWindow::closeSerialPort()
    {
        serial->close();

        actionConnect->setEnabled(true);
        actionDisconnect->setEnabled(false);

        statusbar->showMessage(tr("Disconnected"));
    }

    void hrmMainWindow::getSettings()
    {
        QString data = "get settings\n";

        writeData(data.toLocal8Bit());

        console->print(data);
    }

    void hrmMainWindow::handleError(QSerialPort::SerialPortError error)
    {
        if (error == QSerialPort::ResourceError) {
            QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
            closeSerialPort();
        }
    }

}
