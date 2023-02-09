#ifndef PLOTNETWORKDATA_H
#define PLOTNETWORKDATA_H

#include <QMainWindow>
#include <QtCore/QFile>
#include <QtCore/QStringList>

#include <fstream>
#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <typeinfo>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>
#include <complex.h>
#include <libssh/libssh.h>

#include "qcustomplot.h"
#include "mythread.h"
#include "myserver.h"
#include "serverthread.h"
#include "string"

QT_BEGIN_NAMESPACE
namespace Ui { class plotNetworkData; }
QT_END_NAMESPACE

class plotNetworkData : public QMainWindow
{
    Q_OBJECT

public:
    plotNetworkData(QWidget *parent = nullptr);
    ~plotNetworkData();

private slots:
    void on_plotTypeComboBox_currentIndexChanged(int index);

    void on_lineStyleComboBox_currentIndexChanged(int index);

    void on_clearButton_clicked();

    void on_ControllerComboBox_currentIndexChanged(int index);

    void on_setButton_clicked();

    void on_connectButton_clicked();

    void on_ControllerComboBox_currentIndexChanged(const QString &arg1);

    void on_disconnectButton_clicked();

    void on_runButton_clicked();

    void on_stopButton_clicked();

    void on_saveButton_clicked();

    void on_plotButton_clicked();

    void on_clearButton_2_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_plotTypeComboBox_2_currentIndexChanged(int index);

    void on_ControllerComboBox_2_currentIndexChanged(const QString &arg1);

    void on_ControllerComboBox_2_currentIndexChanged(int index);

    void on_runButton_2_clicked();

private:
    Ui::plotNetworkData *ui;

public slots:
    void plotNewValues(QVector<double> x,QVector<double> y);
    void clearPlot();
};

//extern int Type;

#endif // PLOTNETWORKDATA_H
