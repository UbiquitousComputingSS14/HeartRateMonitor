#include "MainWindow.h"

#include <QMessageBox>
#include <QList>
#include <QTabWidget>
#include <iostream>

#include <cmath>

namespace hrm
{

    MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
    {
        setupUi(this);

        console = new Console();
        mainLayout->layout()->addWidget(console);

        actionConnect->setEnabled(true);
        actionDisconnect->setEnabled(false);

        statusbar->showMessage(tr("Disconnected"));

        initPlots();
        initSignals();

        plotFrequencyIn->setLimit(controller.getFFTProperties().numberOfSamples);
    }

    void MainWindow::initPlots()
    {
        plotBroadband = new minotaur::MouseMonitorPlot(this);
        plotBroadband->init(Qt::blue, "Broadband", "Sample",
                            "Sensor Data", "Broadband brightness", minotaur::LIMITED);
        graphLayout->layout()->addWidget(plotBroadband);

        plotIr = new minotaur::MouseMonitorPlot(this);
        plotIr->init(Qt::red, "IR", "Sample",
                     "Sensor Data", "IR Brightness", minotaur::LIMITED);
        graphLayout->layout()->addWidget(plotIr);

        plotFrequencyIn = new minotaur::MouseMonitorPlot(this);
        plotFrequencyIn->init(Qt::blue, "Time Domain",
                              "Time (k)", "Brightness", "Brightness", minotaur::LIMITED);
        tabEffective->layout()->addWidget(plotFrequencyIn);

        plotFrequencyInPaddedData = new minotaur::MouseMonitorPlot(this);
        plotFrequencyInPaddedData->init(Qt::blue, "Time Domain",
                                        "Time (k)", "Brightness", "Brightness", minotaur::NO_LIMIT);
        tabZeroPadded->layout()->addWidget(plotFrequencyInPaddedData);

        plotFrequencyOut = new minotaur::MouseMonitorPlot(this);
        plotFrequencyOut->init(Qt::darkCyan, "Frequency Spectrum",
                               "Frequency (Hz)", "Amplitude", "Magnitude", minotaur::NO_LIMIT);
        tabMagnitude->layout()->addWidget(plotFrequencyOut);

        plotFrequencyOutComplexData = new minotaur::MouseMonitorPlot(this);
        plotFrequencyOutComplexData->init(Qt::magenta, "Frequency Spectrum",
                                          "Frequency (Hz)", "Amplitude", "Real Part", minotaur::NO_LIMIT);
        tabComplex->layout()->addWidget(plotFrequencyOutComplexData);

        plotFrequencyOutComplexData->addCurve("Imaginary part", Qt::green);
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

        connect(&controller, SIGNAL(sensorSettings(SensorSettings, FFT_properties)),
                this, SLOT(sensorSettings(SensorSettings, FFT_properties)));
        connect(&controller, SIGNAL(sensorData(SensorData)),
                this, SLOT(sensorData(SensorData)));
        connect(&controller, SIGNAL(frequencySpectrum(std::vector<double>&, int)),
                this, SLOT(frequencySpectrum(std::vector<double>&, int)));
    }

    MainWindow::~MainWindow()
    {
        delete plotBroadband;
        delete plotIr;
        delete plotFrequencyIn;
        delete plotFrequencyOut;
        delete console;
    }

    void MainWindow::sensorSettings(
        SensorSettings settings,
        FFT_properties properties)
    {
        sensorEdit->setText(settings.sensor);
        idEdit->setText(settings.id);
        maxValEdit->setText(settings.max);
        minValEdit->setText(settings.min);
        sampleIntervalEdit->setText(settings.sampleInterval);
        resolutionEdit->setText(settings.resolution);
        setSampleIntervalEdit->setText(settings.sampleInterval);

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

    void MainWindow::sensorData(SensorData data)
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

        str = "Sensor> ";
        str += "Broadband: " + QString::number(data.broadband)
               + " Ir: " + QString::number(data.ir);

        console->print(str);
    }

    void MainWindow::frequencySpectrum(
        std::vector<double>& magnitude,
        int peakIndex)
    {
        std::vector<double>& real = controller.getRealPart();
        std::vector<double>& imaginary = controller.getImaginaryPart();
        FFT_properties properties = controller.getFFTProperties();

        frequencyDataEdit->clear();
        timeDataEdit->clear();
        plotFrequencyOut->clear();
        plotFrequencyOutComplexData->clear();

        // Max peak
        displayPeak(peakIndex, magnitude[peakIndex]);

        QVector<double> dataVector;
        QString str;

        // Plot
        for (int i = 1; i <= properties.totalSamples / 2; ++i) {
            dataVector.clear();

            dataVector.append(magnitude[i-1]);
            plotFrequencyOut->updatePlot(controller.indexToFrequency(i), dataVector);

            dataVector.clear();
            dataVector.append(real[i-1]);
            dataVector.append(imaginary[i-1]);
            plotFrequencyOutComplexData->updatePlot(controller.indexToFrequency(i), dataVector);

            str = QString::number(magnitude[i-1]);
            frequencyDataEdit->append(str);
        }

        fftw_complex *inPadded = controller.getIn();
        plotFrequencyInPaddedData->clear();
        for (int i = 0; i < controller.getFFTProperties().numberOfSamples; ++i) {
            dataVector.clear();
            dataVector.append(inPadded[i][0]);
            plotFrequencyInPaddedData->updatePlot(dataVector);
        }
    }

    void MainWindow::displayPeak(int indexMax, double max)
    {
        FFT_properties properties = controller.getFFTProperties();

        double fraction = indexMax / (double) properties.totalSamples;
        double frequency = properties.sampleRate * fraction;
        double bpm = frequency * 60;

        peakIndexEdit->setText(QString::number(indexMax));
        peakFractionEdit->setText(QString::number(fraction));
        peakFrequencyEdit->setText(QString::number(frequency));
        peakAmplitudeEdit->setText(QString::number(max));
        peakBpmEdit->setText(QString::number(bpm));

        lcdNumber->display(bpm);

        plotFrequencyOut->addMarker(
            properties.sampleRate * ((double) indexMax / properties.totalSamples),
            max);
    }

    void MainWindow::openSerialPortClicked()
    {
        if (controller.start()) {
            statusbar->showMessage(tr("Connected"));

            actionConnect->setEnabled(false);
            actionDisconnect->setEnabled(true);
        } else
            QMessageBox::critical(this, tr("Error"), "Could not open serial port.");
    }

    void MainWindow::closeSerialPortClicked()
    {
        controller.stop();

        actionConnect->setEnabled(true);
        actionDisconnect->setEnabled(false);

        statusbar->showMessage(tr("Disconnected"));
    }

    void MainWindow::getSettingsClicked()
    {
        controller.getSensorSettings();
        console->print("> Getting sensor settings");
    }

    void MainWindow::setSampleIntervalClicked()
    {
        controller.setSampleInterval(setSampleIntervalEdit->text());
        console->print("> Setting sample interval to " + setSampleIntervalEdit->text());
    }

}
