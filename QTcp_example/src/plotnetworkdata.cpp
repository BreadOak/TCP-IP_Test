#include "plotnetworkdata.h"
#include "ui_plotnetworkdata.h"

extern int PlotType;       // Plot type Global variable
extern int CtrlMode;       // Ctrl mode Global variable
extern int Controller;     // Controller Global variable
extern double TargetValue; // TargetValue Global variable

plotNetworkData::plotNetworkData(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::plotNetworkData)
{
    ui->setupUi(this);

    ui->customPlot->addGraph();
//    ui->customPlot->setBackground(QBrush(QColor(0, 0, 0, 0)));
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->customPlot->graph(0)->setPen(QPen(Qt::red));
    ui->customPlot->xAxis->setLabel("X");
    ui->customPlot->yAxis->setLabel("Y");
    ui->customPlot->xAxis->setRange(0, 10);
    ui->customPlot->yAxis->setRange(0, 100);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->plotTypeComboBox->setCurrentIndex(0);
    ui->lineStyleComboBox->setCurrentIndex(0);
    ui->stackedWidget->setCurrentIndex(0);
//    ui->Act_value->setText(QHostAddress::Any.toString());
//    ui->portLineEdit->setText("8000");
    QValidator *validator = new QDoubleValidator(this);
    ui->Tar_value->setValidator(validator);
    ui->StepIP->setInputMask("000.000.000.000");

//    MyThread m;
//    connect(&MyThread,SIGNAL(newDataRecieved(QVector<double> x,QVector<double> y)),this,SLOT(plotNewValues(QVector<double> x,QVector<double> y)));
    ServerThread *thread = new ServerThread(this);
    connect(thread,SIGNAL(newDataRecieved(QVector<double> ,QVector<double> )),this,SLOT(plotNewValues(QVector<double> ,QVector<double> )));
    thread->start();
}

plotNetworkData::~plotNetworkData()
{
    delete ui;
}

void plotNetworkData::plotNewValues(QVector<double> x, QVector<double> y)
{
//    qDebug()<< "Got data x:"<<x<<" y:"<<y;
    ui->customPlot->graph(0)->setData(x, y);
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
    ui->customPlot->update();
    ui->Act_value->setText("0");
}

void plotNetworkData::clearPlot()
{
    ui->customPlot->graph(0)->data()->clear();
    ui->customPlot->replot();
    ui->customPlot->update();
}

void plotNetworkData::on_plotTypeComboBox_currentIndexChanged(int index)
{
    switch (index) {
        case 0:
                ui->ControllerComboBox->clear();
                ui->ControllerComboBox->addItems({"PID"});
                break;
        case 1:
                ui->ControllerComboBox->clear();
                ui->ControllerComboBox->addItems({"PI","PI+DOB","MPC","MPC+DOB"});
                break;
        case 2:
                ui->ControllerComboBox->clear();
                ui->ControllerComboBox->addItems({"FirmWare Contoller"});
                break;
    }

    CtrlMode = index;
    ui->customPlot->replot();
    ui->customPlot->update();
}

void plotNetworkData::on_ControllerComboBox_currentIndexChanged(int index)
{
    Controller = index;
    ui->customPlot->replot();
    ui->customPlot->update();
}

void plotNetworkData::on_lineStyleComboBox_currentIndexChanged(int index)
{
    PlotType = index;
    if(PlotType == 0){
        ui->customPlot->graph(0)->setPen(QPen(Qt::red));
    } else if (PlotType == 1) {
        ui->customPlot->graph(0)->setPen(QPen(Qt::green));
    } else if (PlotType == 2) {
        ui->customPlot->graph(0)->setPen(QPen(Qt::blue));
    }
    ui->customPlot->replot();
    ui->customPlot->update();
}

void plotNetworkData::on_clearButton_clicked()
{
    clearPlot();
}

void plotNetworkData::on_setButton_clicked()
{
    TargetValue = ui->Tar_value->text().toDouble();
}

void plotNetworkData::on_connectButton_clicked()
{
    ui->StepIP->text();
    ui->UserID->text();
    ui->Password->text();
}

void plotNetworkData::on_ControllerComboBox_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "PID"){
        ui->stackedWidget->setCurrentIndex(0);
    } else if (arg1 == "PI") {
        ui->stackedWidget->setCurrentIndex(1);
    } else if (arg1 == "PI+DOB") {
        ui->stackedWidget->setCurrentIndex(2);
    } else if (arg1 == "MPC") {
        ui->stackedWidget->setCurrentIndex(3);
    } else if (arg1 == "MPC+DOB") {
        ui->stackedWidget->setCurrentIndex(4);
    } else if (arg1 == "FirmWare Contoller") {
        ui->stackedWidget->setCurrentIndex(5);
    }
}
