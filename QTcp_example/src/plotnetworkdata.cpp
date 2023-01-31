#include "plotnetworkdata.h"
#include "ui_plotnetworkdata.h"

extern int OnOffSignal;
extern int RunSignal;
extern int PlotType;       // Plot type Global variable
extern int CtrlMode;       // Ctrl mode Global variable
extern int Controller;     // Controller Global variable
extern double TargetValue; // TargetValue Global variable

ssh_session my_ssh_session;
ssh_channel channel;
int rc;

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
    ui->Tar_value->setText("0");
    QValidator *validator = new QDoubleValidator(this);
    ui->Tar_value->setValidator(validator);
    ui->StepIP->setInputMask("000.000.000.000");

    RunSignal = 0;

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
    if (RunSignal){
        ui->customPlot->graph(0)->setData(x, y);
        ui->customPlot->rescaleAxes();
        ui->customPlot->replot();
        ui->customPlot->update();
        ui->Act_value->setText("0");
    }
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
    OnOffSignal = 1; // On

    QString Step_IP = ui->StepIP->text();
    QString User_ID = ui->UserID->text();
    QString Pass_Word = ui->Password->text();

    ///// Open session and set options /////
    my_ssh_session = ssh_new();
    if (my_ssh_session == NULL)
      exit(-1);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, Step_IP.toStdString().c_str());
    ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, User_ID.toStdString().c_str());

    ///// Connect to server /////
    rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK)
    {
      fprintf(stderr, "Error connecting to localhost: %s\n",
              ssh_get_error(my_ssh_session));
      ssh_free(my_ssh_session);
      exit(-1);
    }

    ///// Password /////
    char *password;
    password = (char *)Pass_Word.toStdString().c_str();
    rc = ssh_userauth_password(my_ssh_session, NULL, password);
    if (rc != SSH_AUTH_SUCCESS)
    {
      fprintf(stderr, "Error authenticating with password: %s\n",
              ssh_get_error(my_ssh_session));
      ssh_disconnect(my_ssh_session);
      ssh_free(my_ssh_session);
      exit(-1);
    }

    ///// Make Channel /////
    channel = ssh_channel_new(my_ssh_session);
    rc = ssh_channel_open_session(channel);

    ///// Request_exec /////
    rc = ssh_channel_request_exec(channel, "cd release; sudo ./TCP_test_GUI 0 0 3 45");
    if (rc != SSH_OK)
    {
      ssh_channel_close(channel);
      ssh_channel_free(channel);
    }
    ui->ConnectionState->setText("Connect");

}

void plotNetworkData::on_disconnectButton_clicked()
{
    if(ui->ConnectionState->text() == "Connect")
    {
        RunSignal = 0;   // Stop
        OnOffSignal = 0; // Off

        ssh_channel_send_eof(channel);
        ssh_channel_close(channel);
        ssh_channel_free(channel);

        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);

        ui->ConnectionState->setText("Disconnect");
    }
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

void plotNetworkData::on_runButton_clicked()
{
//    OnOffSignal = 1; // On
    TargetValue = ui->Tar_value->text().toDouble();
    RunSignal   = 1; // Run
}
void plotNetworkData::on_stopButton_clicked()
{
    RunSignal = 0; // Stop
}
