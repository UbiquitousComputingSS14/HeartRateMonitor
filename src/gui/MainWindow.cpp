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

        plotFrequencyIn->setLimit(fft.getProperties().numberOfSamples);
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

        plotFrequencyOut->addCurve("Real part", Qt::magenta);
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
        if (!fft.ready()) {
            getSettingsClicked();
            return;
        }

        displayData(data);

        if (fft.addSample(data.broadband))
            displayFrequencies();

        return;

        QString str = "Sensor> ";
        str += "Broadband: " + QString::number(data.broadband)
               + " Ir: " + QString::number(data.ir);

        console->print(str);
    }

    void MainWindow::displayData(SensorData data)
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
    }

    void MainWindow::displayFrequencies()
    {
        frequencyDataEdit->clear();
        timeDataEdit->clear();
        plotFrequencyOut->clear();

        double *magnitude = fft.getMagnitude();
        double *real = fft.getRealPart();
        double *imaginary = fft.getImaginaryPart();

        FFT_properties properties = fft.getProperties();

        // Max peak
        int indexMax = fft.getPeak();
        displayPeak(indexMax, magnitude[indexMax]);

        QVector<double> dataVector;
        QString str;

        // Plot
        for (int i = 1; i <= properties.totalSamples / 2; ++i) {
            /*if (fft.indexToFrequency(i) < MIN_PULSE_FREQUENCY ||
                    fft.indexToFrequency(i) > MAX_PULSE_FREQUENCY)
                continue;*/

            dataVector.clear();

            dataVector.append(magnitude[i-1]);
            dataVector.append(real[i-1]);
            dataVector.append(imaginary[i-1]);

            plotFrequencyOut->updatePlot(fft.indexToFrequency(i), dataVector);

            str = QString::number(magnitude[i-1]);
            frequencyDataEdit->append(str);
        }
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

        fft.setSampleInterval(settings.sampleInterval.toDouble());
        FFT_properties properties = fft.getProperties();

        fftSampleIntervalEdit->setText(QString::number(properties.sampleInterval));
        fftSampleRateEdit->setText(QString::number(properties.sampleRate));
        fftSamplesPerSegmentEdit->setText(QString::number(properties.numberOfSamples));
        fftZeroPaddingEdit->setText(QString::number(properties.zeroPaddingSamples));
        fftSegmentDurationEdit->setText(QString::number(properties.segmentDuration));

        QString str;

        str = QString::number(properties.frequencyResolution) + " (" +
              QString::number(properties.frequencyResolution * 60) + " bpm)";

        fftFrequencyResolutionEdit->setText(str);

        str = QString::number(properties.frequencyResolutionWithZeroPadding) + " (" +
              QString::number(properties.frequencyResolutionWithZeroPadding * 60) + " bpm)";

        fftFrequencyResolutionZPEdit->setText(str);
    }

    void MainWindow::openSerialPortClicked()
    {
        if (serial->openSerial()) {
            statusbar->showMessage(tr("Connected"));

            actionConnect->setEnabled(false);
            actionDisconnect->setEnabled(true);

            getSettingsClicked();
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

        getSettingsClicked();
    }

    void MainWindow::displayPeak(int indexMax, double max)
    {
        FFT_properties properties = fft.getProperties();

        double fraction = indexMax / (double) properties.totalSamples;
        double frequency = properties.sampleRate * fraction;
        double bpm = frequency * 60;

        peakIndexEdit->setText(QString::number(indexMax));
        peakFractionEdit->setText(QString::number(fraction));
        peakFrequencyEdit->setText(QString::number(frequency));
        peakAmplitudeEdit->setText(QString::number(max));
        peakBpmEdit->setText(QString::number(bpm));

        plotFrequencyOut->addMarker(
            properties.sampleRate * ((double) indexMax / properties.totalSamples),
            max);
    }

}
