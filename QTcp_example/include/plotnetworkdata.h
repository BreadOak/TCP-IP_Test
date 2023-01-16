#ifndef PLOTNETWORKDATA_H
#define PLOTNETWORKDATA_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "mythread.h"
#include "myserver.h"
#include "serverthread.h"

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

private:
    Ui::plotNetworkData *ui;

public slots:
    void plotNewValues(QVector<double> x,QVector<double> y);
};
#endif // PLOTNETWORKDATA_H
