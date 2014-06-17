#include "MainWindow.h"

#include <QMessageBox>
#include <QList>

namespace hrm
{

    MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
    {
        setupUi(this);

        plotBroadband = new minotaur::MouseMonitorPlot();
        plotBroadband->init(Qt::blue, "Broadband", "Sample",
                            "Sensor Data", minotaur::LIMITED);
        graphLayout->layout()->addWidget(plotBroadband);

        plotIr = new minotaur::MouseMonitorPlot();
        plotIr->init(Qt::red, "IR", "Sample",
                     "Sensor Data", minotaur::LIMITED);
        graphLayout->layout()->addWidget(plotIr);

        plotFrequencyIn = new minotaur::MouseMonitorPlot();
        plotFrequencyIn->init(Qt::blue, "Discrete Function",
                              "Time", "Value", minotaur::NO_LIMIT);
        frequencyGraphLayout->layout()->addWidget(plotFrequencyIn);

        plotFrequencyOut = new minotaur::MouseMonitorPlot();
        plotFrequencyOut->init(Qt::red, "Frequency Spectrum",
                               "Frequency", "Magnitude", minotaur::NO_LIMIT);
        frequencyGraphLayout->layout()->addWidget(plotFrequencyOut);

        console = new Console();
        mainLayout->layout()->addWidget(console);

        serial = new Serial();

        actionConnect->setEnabled(true);
        actionDisconnect->setEnabled(false);

        connect(actionConnect, SIGNAL(triggered()),
                this, SLOT(openSerialPortClicked()));
        connect(actionDisconnect, SIGNAL(triggered()),
                this, SLOT(closeSerialPortClicked()));
        connect(getSettingsBtn, SIGNAL(clicked()),
                this, SLOT(getSettingsClicked()));
        connect(setSampleIntervalBtn, SIGNAL(clicked()),
                this, SLOT(setSampleIntervalClicked()));

        connect(serial, SIGNAL(receiveLine(QString)),
                this, SLOT(receiveLine(QString)));
        connect(serial, SIGNAL(receiveSensorData(SensorData)),
                this, SLOT(receiveSensorData(SensorData)));
        connect(serial, SIGNAL(receiveSensorSettings(SensorSettings)),
                this, SLOT(receiveSensorSettings(SensorSettings)));

        statusbar->showMessage(tr("Disconnected"));


        FFT fft;
        fft.computeFFT();

        fftw_complex *in = fft.getIn();
        for (int i = 0; i < 256; ++i) {
            plotFrequencyIn->updatePlot(in[i][0]);
        }

        fftw_complex *out = fft.getOut();
        for (int i = 0; i < 256; ++i) {
            plotFrequencyOut->updatePlot(out[i][0]);
        }
    }

    MainWindow::~MainWindow()
    {
        serial->closeSerial();

        delete plotBroadband;
        delete plotIr;
        delete serial;
        delete console;
    }

    void MainWindow::receiveLine(QString line)
    {
        /*plotBroadband->updatePlot();
        plotIr->updatePlot();*/
    }

    void MainWindow::receiveSensorData(SensorData data)
    {
        plotBroadband->updatePlot(data.broadband);
        plotIr->updatePlot(data.ir);

        QString str = "Sensor> ";
        str += "Broadband: " + QString::number(data.broadband)
               + " Ir: " + QString::number(data.ir);

        console->print(str);
    }

    void MainWindow::receiveSensorSettings(SensorSettings settings)
    {
        sensorEdit->setText(settings.sensor);
        idEdit->setText(settings.id);
        maxValEdit->setText(settings.max);
        minValEdit->setText(settings.min);
        sampleIntervalEdit->setText(settings.sampleInterval);
        resolutionEdit->setText(settings.resolution);
    }

    void MainWindow::openSerialPortClicked()
    {
        if (serial->openSerial()) {
            statusbar->showMessage(tr("Connected"));

            actionConnect->setEnabled(false);
            actionDisconnect->setEnabled(true);
        } else
            QMessageBox::critical(this, tr("Error"), serial->errorString());
    }

    void MainWindow::closeSerialPortClicked()
    {
        serial->closeSerial();

        actionConnect->setEnabled(true);
        actionDisconnect->setEnabled(false);

        statusbar->showMessage(tr("Disconnected"));
    }

    void MainWindow::getSettingsClicked()
    {
        QString data = "get settings";
        serial->sendData(data + '\n');
        console->print("> " + data);
    }

    void MainWindow::setSampleIntervalClicked()
    {
        QString data = "set sampleInterval " + setSampleIntervalEdit->text();
        serial->sendData(data + "\n");
        console->print("> " + data);
    }

}
