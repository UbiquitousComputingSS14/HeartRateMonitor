#include "MainWindow.h"

#include <QMessageBox>
#include <QList>

namespace hrm
{

    MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
    {
        setupUi(this);

        plotBroadband = new minotaur::MouseMonitorPlot();
        plotBroadband->init(Qt::blue, "Broadband", "Sample", "Sensor Data");
        graphLayout->layout()->addWidget(plotBroadband);

        plotIr = new minotaur::MouseMonitorPlot();
        plotIr->init(Qt::red, "IR", "Sample", "Sensor Data");
        graphLayout->layout()->addWidget(plotIr);

        console = new Console();
        mainLayout->layout()->addWidget(console);

        serial = new Serial();

        actionConnect->setEnabled(true);
        actionDisconnect->setEnabled(false);

        connect(actionConnect, SIGNAL(triggered()), this, SLOT(openSerialPortClicked()));
        connect(actionDisconnect, SIGNAL(triggered()), this, SLOT(closeSerialPortClicked()));
        connect(getSettingsBtn, SIGNAL(clicked()), this, SLOT(getSettingsClicked()));
        connect(setSampleIntervalBtn, SIGNAL(clicked()), this, SLOT(setSampleIntervalClicked()));

        connect(serial, SIGNAL(receiveLine(QString)), this, SLOT(receiveLine(QString)));

        statusbar->showMessage(tr("Disconnected"));
    }

    MainWindow::~MainWindow()
    {
        serial->closeSerial();

        delete plotBroadband;
        delete plotIr;
        delete serial;
        delete console;
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

    void MainWindow::receiveLine(QString line)
    {
        /*plotBroadband->updatePlot();
        plotIr->updatePlot();*/
    }

    void MainWindow::getSettingsClicked()
    {
        QString data = "get settings\n";
        serial->sendData(data);
        console->print(data);
    }

    void MainWindow::setSampleIntervalClicked()
    {
        QString data = "set sampleInterval " + setSampleIntervalEdit->text() + "\n";
        serial->sendData(data);
        console->print(data);
    }

}
