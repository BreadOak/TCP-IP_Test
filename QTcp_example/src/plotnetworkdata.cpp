#include "plotnetworkdata.h"
#include "ui_plotnetworkdata.h"

extern int OnOffSignal;
extern int RunSignal;
extern int PlotType;       // Plot type Global variable
extern int CtrlMode;       // Ctrl mode Global variable
extern int Controller;     // Controller Global variable
extern double TargetValue; // TargetValue Global variable
extern double Kp_PosCtrl;
extern double Ki_PosCtrl;
extern double Kd_PosCtrl;
extern double Kp_VelCtrl;
extern double Ki_VelCtrl;
extern int DOB_option;

ssh_session my_ssh_session;
ssh_channel channel;

QString HOST;
QString PORT;

int rc;
int MaxVel = 150;
int MaxTor = 10;

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
    ui->Tar_value->setText("0");
    QValidator *validator = new QDoubleValidator(this);
    ui->Tar_value->setValidator(validator);

    ui->IPAddLabel->setText("192.168.0.6");
    ui->PortLabel->setText("8000");

    ui->StepIP->setText("192.168.0.39");
    ui->UserID->setText("user");
    ui->Password->setText("nrmk2013");

    // Initial Gain setting
    ui->Kp_pos->setMaximum(10000);
    ui->Ki_pos->setMaximum(10000);
    ui->Kd_pos->setMaximum(10000);
    ui->Kp_pos->setValue(1000.0);
    ui->Ki_pos->setValue(10.0);
    ui->Kd_pos->setValue(10.0);

    ui->Kp_vel->setMaximum(10000);
    ui->Ki_vel->setMaximum(10000);
    ui->Kp_vel->setValue(300.0);
    ui->Ki_vel->setValue(200.0);
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
        ui->Act_value->setText("123.4");
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
                ui->ControllerComboBox->addItems({"PI","MPC","H-inf"});
                break;
        case 2:
                ui->ControllerComboBox->clear();
                ui->ControllerComboBox->addItems({"FirmWare Contoller"});
                break;
    }

    CtrlMode = index;

    // Limit the TargetValue
    if (CtrlMode==1){
        if(TargetValue > MaxVel){
            TargetValue = MaxVel;
            ui->Tar_value->setText(QString::number(MaxVel));
        }
        if(TargetValue < -MaxVel){
            TargetValue = -MaxVel;
            ui->Tar_value->setText(QString::number(-MaxVel));
        }
    }
    if (CtrlMode==2){
        if(TargetValue > MaxTor){
            TargetValue = MaxTor;
            ui->Tar_value->setText(QString::number(MaxTor));
        }
        if(TargetValue < -MaxTor){
            TargetValue = -MaxTor;
            ui->Tar_value->setText(QString::number(-MaxTor));
        }
    }

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

    // Limit the TargetValue
    if (CtrlMode==1){
        if(TargetValue > MaxVel){
            TargetValue = MaxVel;
            ui->Tar_value->setText(QString::number(MaxVel));
        }
        if(TargetValue < -MaxVel){
            TargetValue = -MaxVel;
            ui->Tar_value->setText(QString::number(-MaxVel));
        }
    }
    if (CtrlMode==2){
        if(TargetValue > MaxTor){
            TargetValue = MaxTor;
            ui->Tar_value->setText(QString::number(MaxTor));
        }
        if(TargetValue < -MaxTor){
            TargetValue = -MaxTor;
            ui->Tar_value->setText(QString::number(-MaxTor));
        }
    }

    Kp_PosCtrl = ui->Kp_pos->text().toDouble();
    Ki_PosCtrl = ui->Ki_pos->text().toDouble();
    Kd_PosCtrl = ui->Kd_pos->text().toDouble();
    Kp_VelCtrl = ui->Kp_vel->text().toDouble();
    Ki_VelCtrl = ui->Ki_vel->text().toDouble();

    if( (ui->DOB_option_PI->isChecked() && Controller == 0) || (ui->DOB_option_MPC->isChecked() && Controller == 1) || (ui->DOB_option_Hinf->isChecked() && Controller == 2) )
    {
        DOB_option = 1;
    }
    else{
        DOB_option = 0;
    }
    qDebug()  << "DOB_option:"<< DOB_option;
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
        ui->ConnectionState->setText("Disconnect");
    }

    ///// Password /////
    char *password;
    password = (char *)Pass_Word.toStdString().c_str();
    rc = ssh_userauth_password(my_ssh_session, NULL, password);
    if (rc != SSH_AUTH_SUCCESS)
    {
        ui->ConnectionState->setText("Disconnect");
    }

    ///// Make Channel /////
    channel = ssh_channel_new(my_ssh_session);
    rc = ssh_channel_open_session(channel);

    ///// Request_exec /////
    QString Empt = " ";
    QString Command = "cd release; sudo ./TCP_test_GUI 0 0" + Empt + HOST + Empt + PORT;

    rc = ssh_channel_request_exec(channel, Command.toStdString().c_str());
    if (rc != SSH_OK)
    {
        ui->ConnectionState->setText("Disconnect");
    }
    else{
        ui->ConnectionState->setText("Connect");
    }

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
    } else if (arg1 == "MPC") {
        ui->stackedWidget->setCurrentIndex(2);
    } else if (arg1 == "H-inf") {
        ui->stackedWidget->setCurrentIndex(3);
    } else if (arg1 == "FirmWare Contoller") {
        ui->stackedWidget->setCurrentIndex(4);
    }
}

void plotNetworkData::on_runButton_clicked()
{
//    OnOffSignal = 1; // On
    TargetValue = ui->Tar_value->text().toDouble();

    // Limit the TargetValue
    if (CtrlMode==1){
        if(TargetValue > MaxVel){
            TargetValue = MaxVel;
            ui->Tar_value->setText(QString::number(MaxVel));
        }
        if(TargetValue < -MaxVel){
            TargetValue = -MaxVel;
            ui->Tar_value->setText(QString::number(-MaxVel));
        }
    }
    if (CtrlMode==2){
        if(TargetValue > MaxTor){
            TargetValue = MaxTor;
            ui->Tar_value->setText(QString::number(MaxTor));
        }
        if(TargetValue < -MaxTor){
            TargetValue = -MaxTor;
            ui->Tar_value->setText(QString::number(-MaxTor));
        }
    }

    Kp_PosCtrl = ui->Kp_pos->text().toDouble();
    Ki_PosCtrl = ui->Ki_pos->text().toDouble();
    Kd_PosCtrl = ui->Kd_pos->text().toDouble();
    Kp_VelCtrl = ui->Kp_vel->text().toDouble();
    Ki_VelCtrl = ui->Ki_vel->text().toDouble();

    if( (ui->DOB_option_PI->isChecked() && Controller == 0) || (ui->DOB_option_MPC->isChecked() && Controller == 1) || (ui->DOB_option_Hinf->isChecked() && Controller == 2) )
    {
        DOB_option = 1;
    }
    else{
        DOB_option = 0;
    }

    RunSignal  = 1; // Run
}
void plotNetworkData::on_stopButton_clicked()
{
    RunSignal = 0; // Stop
}

void plotNetworkData::on_saveButton_clicked()
{
    HOST = ui->IPAddLabel->text();
    PORT = ui->PortLabel->text();
    ui->Saved->setText("Saved!");
}

void plotNetworkData::on_DOB_option_PI_stateChanged(int arg1)
{

}
