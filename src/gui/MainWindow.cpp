#include "MainWindow.h"

#include <QMessageBox>
#include <QList>

#include <cmath>

namespace hrm
{

    MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
    {
        setupUi(this);

        console = new Console();
        mainLayout->layout()->addWidget(console);

        serial = new Serial();

        actionConnect->setEnabled(true);
        actionDisconnect->setEnabled(false);

        statusbar->showMessage(tr("Disconnected"));

        initPlots();
        initSignals();

        plotFrequencyIn->setSticksStyle();
        plotFrequencyIn->setLimit(fft.getN());

        //plotFrequencyOut->setSticksStyle();
    }

    void MainWindow::initPlots()
    {
        plotBroadband = new minotaur::MouseMonitorPlot();
        plotBroadband->init(Qt::blue, "Broadband", "Sample",
                            "Sensor Data", "Broadband brightness", minotaur::LIMITED);
        graphLayout->layout()->addWidget(plotBroadband);

        plotIr = new minotaur::MouseMonitorPlot();
        plotIr->init(Qt::red, "IR", "Sample",
                     "Sensor Data", "IR Brightness", minotaur::LIMITED);
        graphLayout->layout()->addWidget(plotIr);

        plotFrequencyIn = new minotaur::MouseMonitorPlot();
        plotFrequencyIn->init(Qt::blue, "Time Domain",
                              "Time (k)", "Brightness", "Brightness", minotaur::LIMITED);
        frequencyGraphLayout->layout()->addWidget(plotFrequencyIn);

        plotFrequencyOut = new minotaur::MouseMonitorPlot();
        plotFrequencyOut->init(Qt::black, "Frequency Spectrum",
                               "Frequency (Hz)", "Amplitude", "Magnitude (polar coordinate)", minotaur::NO_LIMIT);
        frequencyGraphLayout->layout()->addWidget(plotFrequencyOut);

        plotFrequencyOut->addCurve("Real part", Qt::red);
        plotFrequencyOut->addCurve("Imaginary part", Qt::green);
    }

    void MainWindow::initSignals()
    {
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
    }

    MainWindow::~MainWindow()
    {
        serial->closeSerial();

        delete plotBroadband;
        delete plotIr;
        delete plotFrequencyIn;
        delete plotFrequencyOut;
        delete serial;
        delete console;
    }

    void MainWindow::receiveLine(QString line)
    {
    }

    void MainWindow::receiveSensorData(SensorData data)
    {
        QString str;

        QVector<double> dataVector;
        dataVector.append(data.broadband);

        plotBroadband->updatePlot(dataVector);
        plotFrequencyIn->updatePlot(dataVector);

        dataVector.clear();
        dataVector.append(data.ir);
        plotIr->updatePlot(dataVector);

        str = QString::number(data.broadband);
        timeDataEdit->append(str);

        if (fft.addSample(data.broadband)) {
            frequencyDataEdit->clear();
            timeDataEdit->clear();
            fftw_complex *data = fft.getOut();

            plotFrequencyOut->clear();
            // Start with 1 (no DC offset); TODO: DC offset problem?
            for (int i = 1; i < fft.getN()/2 - 1; ++i) {

                if (10.0*(i/64.0) < 0.5 || 10.0*(i/64.0) > 4) // TODO: (double) fft.getN()
                    continue;

                //    ;   ABS  ; 2*;
                dataVector.clear();

                // Complex value to magnitude
                double scaledAmplReal = data[i][0]/fft.getN();
                double scaledAmplImag = data[i][1]/fft.getN();
                double magnitude = sqrt(pow(scaledAmplReal, 2) + pow(scaledAmplImag, 2));

                dataVector.append(2*magnitude); // 2* because of + and - frequency
                dataVector.append(data[i][0]/fft.getN());
                dataVector.append(data[i][1]/fft.getN());
                plotFrequencyOut->updatePlot(10.0*(i/64.0), dataVector); // TODO: (double) fft.getN()

                str = QString::number(data[i][0]);
                frequencyDataEdit->append(str);
            }
        }

        str = "Sensor> ";
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

        setSampleIntervalEdit->setText(settings.sampleInterval);
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
