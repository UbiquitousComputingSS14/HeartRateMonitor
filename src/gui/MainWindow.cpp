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

        settingsDialog = new SettingsDialog(this);

        actionConnect->setEnabled(true);
        actionDisconnect->setEnabled(false);

        statusbar->showMessage(tr("Disconnected"));

        initPlots();
        initSignals();
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
        connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
        connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

        connect(actionConnect, SIGNAL(triggered()),
                this, SLOT(openSerialPortClicked()));
        connect(actionDisconnect, SIGNAL(triggered()),
                this, SLOT(closeSerialPortClicked()));
        connect(actionSettings, SIGNAL(triggered()),
                this, SLOT(openSettingsDialogClicked()));
        connect(actionSettings_2, SIGNAL(triggered()),
                this, SLOT(openSettingsDialogClicked()));

        // From settings dialog
        connect(settingsDialog->getSettingsBtn, SIGNAL(clicked()),
                this, SLOT(getSettingsClicked()));

        connect(sampleIntervalSlider, SIGNAL(sliderReleased()),
                this, SLOT(sampleIntervalSliderReleased()));
        connect(effectiveSamplesSlider, SIGNAL(sliderReleased()),
                this, SLOT(effectiveSamplesSliderReleased()));
        connect(zeroPaddingSamplesSlider, SIGNAL(sliderReleased()),
                this, SLOT(zeroPaddingSamplesSliderReleased()));
        connect(slidingWindowSlider, SIGNAL(sliderReleased()),
                this, SLOT(slidingWindowSliderReleased()));
        connect(sampleIntervalSlider, SIGNAL(valueChanged(int)),
                this, SLOT(sampleIntervalSliderChanged(int)));
        connect(effectiveSamplesSlider, SIGNAL(valueChanged(int)),
                this, SLOT(effectiveSamplesSliderChanged(int)));
        connect(zeroPaddingSamplesSlider, SIGNAL(valueChanged(int)),
                this, SLOT(zeroPaddingSamplesSliderChanged(int)));
        connect(slidingWindowSlider, SIGNAL(valueChanged(int)),
                this, SLOT(slidingWindowSliderChanged(int)));

        connect(windowFunctionCheckBox, SIGNAL(stateChanged(int)),
                this, SLOT(windowFunctionCheckBoxChanged(int)));
        connect(filterCheckBox, SIGNAL(stateChanged(int)),
                this, SLOT(filterCheckBoxChanged(int)));
        connect(scalingCheckBox, SIGNAL(stateChanged(int)),
                this, SLOT(scalingCheckBoxChanged(int)));

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
        delete settingsDialog;
    }

    void MainWindow::sensorSettings(
        SensorSettings settings,
        FFT_properties properties)
    {
        settingsDialog->setSensorInfo(settings);
        settingsDialog->setFFTInfo(properties);

        plotFrequencyIn->setLimit(properties.numberOfSamples);

        sampleIntervalSlider->setValue(properties.sampleInterval);
        effectiveSamplesSlider->setValue(properties.numberOfSamples);
        zeroPaddingSamplesSlider->setValue(properties.zeroPaddingSamples);
        slidingWindowSlider->setValue(properties.slidingWindow);
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

        settingsDialog->setTimeDataEdit(data.broadband);

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

        settingsDialog->clearFrequencyDataEdit();
        settingsDialog->clearTimeDataEdit();
        plotFrequencyOut->clear();
        plotFrequencyOutComplexData->clear();

        // Max peak
        displayPeak(peakIndex, magnitude[peakIndex]);

        QVector<double> dataVector;
        QString str;

        // Plot
        for (int i = 1; i <= properties.totalSamples / 2; ++i) {
            if (!controller.isRequiredFrequency(i))
                continue;

            dataVector.clear();

            dataVector.append(magnitude[i-1]);
            plotFrequencyOut->updatePlot(controller.indexToFrequency(i), dataVector);

            dataVector.clear();
            dataVector.append(real[i-1]);
            dataVector.append(imaginary[i-1]);
            plotFrequencyOutComplexData->updatePlot(controller.indexToFrequency(i), dataVector);

            settingsDialog->setFrequencyDataEdit(magnitude[i-1]);
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

        settingsDialog->setPeakInfo(indexMax, fraction, frequency, max, bpm);

        lcdNumber->display(bpm);

        plotFrequencyOut->addMarker(frequency, max);
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
        console->printInfo("> Getting sensor settings");
    }

    void MainWindow::openSettingsDialogClicked()
    {
        settingsDialog->setVisible(true);
    }

    /**
     * Sets sample interval on light sensor.
     *
     * Command is sent via serial to the sensor. After that the sensors'
     * settings are queried. A signal is emmited from serial class which
     * sets the GUI info elements.
     */
    void MainWindow::sampleIntervalSliderReleased()
    {
        controller.setSampleInterval(QString::number(sampleIntervalSlider->value()));
        console->printInfo("> Setting sample interval to " +
                           QString::number(sampleIntervalSlider->value()));
    }

    void MainWindow::effectiveSamplesSliderReleased()
    {
        controller.setEffectiveSize(effectiveSamplesSlider->value());
        console->printInfo("> Setting effective sample size to " +
                           QString::number(effectiveSamplesSlider->value()));

        settingsDialog->setFFTInfo(controller.getFFTProperties());
        plotFrequencyIn->setLimit(controller.getFFTProperties().numberOfSamples);
        plotFrequencyIn->clear();
    }

    void MainWindow::zeroPaddingSamplesSliderReleased()
    {
        controller.setZeroPadSize(zeroPaddingSamplesSlider->value());
        console->printInfo("> Setting zero padding sample size to " +
                           QString::number(zeroPaddingSamplesSlider->value()));

        settingsDialog->setFFTInfo(controller.getFFTProperties());
        plotFrequencyIn->setLimit(controller.getFFTProperties().numberOfSamples);
        plotFrequencyIn->clear();
    }

    void MainWindow::slidingWindowSliderReleased()
    {
        controller.setSlidingWindowSize(slidingWindowSlider->value());
        console->printInfo("> Setting sliding window size to " +
                           QString::number(slidingWindowSlider->value()));

        settingsDialog->setFFTInfo(controller.getFFTProperties());
        plotFrequencyIn->setLimit(controller.getFFTProperties().numberOfSamples);
        plotFrequencyIn->clear();
    }

    void MainWindow::sampleIntervalSliderChanged(int value)
    {
        sampleIntervalEdit->setText(QString::number(value));
    }

    void MainWindow::effectiveSamplesSliderChanged(int value)
    {
        effectiveSamplesEdit->setText(QString::number(value));
    }

    void MainWindow::zeroPaddingSamplesSliderChanged(int value)
    {
        zeroPaddingSamplesEdit->setText(QString::number(value));
    }

    void MainWindow::slidingWindowSliderChanged(int value)
    {
        slidingWindowEdit->setText(QString::number(value));
    }

    void MainWindow::windowFunctionCheckBoxChanged(int state)
    {
        controller.setUseWindowFunction(state);
    }

    void MainWindow::filterCheckBoxChanged(int state)
    {
        controller.setUseFilter(state);
    }

    void MainWindow::scalingCheckBoxChanged(int state)
    {
        controller.setUseScaling(state);
    }

    void MainWindow::about()
    {
        QMessageBox::about(this, tr("About HRM"),
                           "HRM measures the users' heart rate with the photoplethysmogram technique. "
                           "It uses the FFT to identify the frequency spectrum and detect the current heart rate. "
                           "More information are available at Github.\n\n"
                           "Author: Jens Gansloser\n"
                           "Github: https://github.com/UbiquitousComputingSS14/Documentation");
    }

}
