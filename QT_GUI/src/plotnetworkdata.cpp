#include "plotnetworkdata.h"
#include "ui_plotnetworkdata.h"

extern int OnOffSignal;
extern int RunSignal;
extern int PlotType;       // Plot type Global variable
extern int CtrlMode;       // Ctrl mode Global variable
extern int Controller;     // Controller Global variable
extern int DOB_option;
extern double TargetValue; // TargetValue Global variable
extern double Kp_PosCtrl;
extern double Ki_PosCtrl;
extern double Kd_PosCtrl;
extern double Kp_VelCtrl;
extern double Ki_VelCtrl;

ssh_session my_ssh_session;
ssh_channel channel;

ssh_scp scp;
ssh_session BP_session;
ssh_channel BPchannel;

QString HOST;
QString PORT;

double Endtime;
int Mode;
int rc;
int MaxVel = 150;
int MaxTor = 10;

static const double PI = 3.141592653589793;

using namespace std;

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
    ui->customPlot->xAxis->setLabel("Time[s]");
    ui->customPlot->yAxis->setLabel("Actual Value");
    ui->customPlot->xAxis->setRange(0, 10);
    ui->customPlot->yAxis->setRange(0, 100);
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->MagPlot->addGraph();
    ui->MagPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);
    ui->MagPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->MagPlot->graph(0)->setPen(QPen(Qt::black));
    ui->MagPlot->xAxis->setLabel("Freq[Hz]");
    ui->MagPlot->yAxis->setLabel("Magnitude");
    ui->MagPlot->xAxis->setRange(0, 10000);
    ui->MagPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->MagPlot->yAxis->setRange(-10, 10);
    ui->MagPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    ui->PhasePlot->addGraph();
    ui->PhasePlot->graph(0)->setScatterStyle(QCPScatterStyle::ssNone);
    ui->PhasePlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->PhasePlot->graph(0)->setPen(QPen(Qt::black));
    ui->PhasePlot->xAxis->setLabel("Freq[Hz]");
    ui->PhasePlot->yAxis->setLabel("Phase");
    ui->PhasePlot->xAxis->setRange(0, 10000);
    ui->PhasePlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->PhasePlot->yAxis->setRange(-10, 10);
    ui->PhasePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    ui->MagPlot->xAxis->setTicker(logTicker);
    ui->PhasePlot->xAxis->setTicker(logTicker);

    ui->plotTypeComboBox->setCurrentIndex(0);
    ui->plotTypeComboBox_2->setCurrentIndex(0);
    ui->lineStyleComboBox->setCurrentIndex(0);
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);
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

    ui->Kp_pos_2->setMaximum(10000);
    ui->Ki_pos_2->setMaximum(10000);
    ui->Kd_pos_2->setMaximum(10000);
    ui->Kp_pos_2->setValue(1000.0);
    ui->Ki_pos_2->setValue(10.0);
    ui->Kd_pos_2->setValue(10.0);

    ui->Kp_vel->setMaximum(10000);
    ui->Ki_vel->setMaximum(10000);
    ui->Kp_vel->setValue(300.0);
    ui->Ki_vel->setValue(200.0);

    ui->Kp_vel_2->setMaximum(10000);
    ui->Ki_vel_2->setMaximum(10000);
    ui->Kp_vel_2->setValue(300.0);
    ui->Ki_vel_2->setValue(200.0);

    ui->Kp_vel->setMaximum(100);
    ui->Nc_vel->setMaximum(25);
    ui->rw_vel->setMaximum(10);
    ui->rw_vel->setSingleStep(0.01);
    ui->Np_vel->setValue(100.0);
    ui->Nc_vel->setValue(25.0);
    ui->rw_vel->setValue(0.01);

    ui->Np_vel_2->setMaximum(100);
    ui->Nc_vel_2->setMaximum(25);
    ui->rw_vel_2->setMaximum(10);
    ui->rw_vel_2->setSingleStep(0.01);
    ui->Np_vel_2->setValue(100.0);
    ui->Nc_vel_2->setValue(25.0);
    ui->rw_vel_2->setValue(0.01);

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
        ui->Act_value->setText(QString::number(y.last()));
    }
    if ((x.last() > 10) && Mode == 1){
        ui->State_2->setText("Finished");
    }
}

void plotNetworkData::clearPlot()
{
    ui->customPlot->graph(0)->data()->clear();
    ui->customPlot->replot();
    ui->customPlot->update();
}

void plotNetworkData::on_clearButton_2_clicked()
{
    ui->MagPlot->graph(0)->data()->clear();
    ui->MagPlot->replot();
    ui->MagPlot->update();
    ui->PhasePlot->graph(0)->data()->clear();
    ui->PhasePlot->replot();
    ui->PhasePlot->update();
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

void plotNetworkData::on_plotTypeComboBox_2_currentIndexChanged(int index)
{
    switch (index) {
        case 0:
                ui->ControllerComboBox_2->clear();
                ui->ControllerComboBox_2->addItems({"PID"});
                break;
        case 1:
                ui->ControllerComboBox_2->clear();
                ui->ControllerComboBox_2->addItems({"PI","MPC","H-inf"});
                break;
        case 2:
                ui->ControllerComboBox_2->clear();
                ui->ControllerComboBox_2->addItems({"FirmWare Contoller"});
                break;
    }

    CtrlMode = index;
    qDebug()  << "CtrlMode:"<< CtrlMode;
}

void plotNetworkData::on_ControllerComboBox_currentIndexChanged(int index)
{
    Controller = index;
    ui->customPlot->replot();
    ui->customPlot->update();
}

void plotNetworkData::on_ControllerComboBox_2_currentIndexChanged(int index)
{
    Controller = index;
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

    if((ui->DOB_option_PI->isChecked() && Controller == 0) || (ui->DOB_option_MPC->isChecked() && Controller == 1) || (ui->DOB_option_Hinf->isChecked() && Controller == 2)){
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
    if (rc != SSH_OK){
        ui->ConnectionState->setText("Disconnect");
    }

    ///// Password /////
    char *password;
    password = (char *)Pass_Word.toStdString().c_str();
    rc = ssh_userauth_password(my_ssh_session, NULL, password);
    if (rc != SSH_AUTH_SUCCESS){
        ui->ConnectionState->setText("Disconnect");
    }

    ///// Make Channel /////
    channel = ssh_channel_new(my_ssh_session);
    rc = ssh_channel_open_session(channel);

    ///// Request_exec /////
    QString Empt = " ";
    QString Np = ui->Np_vel->text();
    QString Nc = ui->Nc_vel->text();
    QString rw = ui->rw_vel->text();
    QString Command = "cd release; sudo ./TCP_test_GUI 0 0" + Empt + HOST + Empt + PORT + Empt + Np + Empt + Nc + Empt + rw;

    rc = ssh_channel_request_exec(channel, Command.toStdString().c_str());
    if (rc != SSH_OK){
        ui->ConnectionState->setText("Disconnect");
    }
    else{
        ui->ConnectionState->setText("Connect");
    }

}

void plotNetworkData::on_disconnectButton_clicked()
{
    if(ui->ConnectionState->text() == "Connect"){
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

void plotNetworkData::on_ControllerComboBox_2_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "PID"){
        ui->stackedWidget_2->setCurrentIndex(0);
    } else if (arg1 == "PI") {
        ui->stackedWidget_2->setCurrentIndex(1);
    } else if (arg1 == "MPC") {
        ui->stackedWidget_2->setCurrentIndex(2);
    } else if (arg1 == "H-inf") {
        ui->stackedWidget_2->setCurrentIndex(3);
    } else if (arg1 == "FirmWare Contoller") {
        ui->stackedWidget_2->setCurrentIndex(4);
    }
}

void plotNetworkData::on_runButton_clicked()
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

    if((ui->DOB_option_PI->isChecked() && Controller == 0) || (ui->DOB_option_MPC->isChecked() && Controller == 1) || (ui->DOB_option_Hinf->isChecked() && Controller == 2)){
        DOB_option = 1;
    }
    else{
        DOB_option = 0;
    }

    RunSignal  = 1; // Run
}

void plotNetworkData::on_runButton_2_clicked()
{
    OnOffSignal = 1; // On
    RunSignal  = 1;  // Run

    Kp_PosCtrl = ui->Kp_pos_2->text().toDouble();
    Ki_PosCtrl = ui->Ki_pos_2->text().toDouble();
    Kd_PosCtrl = ui->Kd_pos_2->text().toDouble();
    Kp_VelCtrl = ui->Kp_vel_2->text().toDouble();
    Ki_VelCtrl = ui->Ki_vel_2->text().toDouble();

    QString Step_IP = ui->StepIP->text();
    QString User_ID = ui->UserID->text();
    QString Pass_Word = ui->Password->text();

    ///// Open session and set options /////
    BP_session = ssh_new();
    if (BP_session == NULL)
      exit(-1);
    ssh_options_set(BP_session, SSH_OPTIONS_HOST, Step_IP.toStdString().c_str());
    ssh_options_set(BP_session, SSH_OPTIONS_USER, User_ID.toStdString().c_str());

    ///// Connect to server /////
    rc = ssh_connect(BP_session);
    if (rc != SSH_OK){
        ui->ConnectionState->setText("Disconnect");
    }

    ///// Password /////
    char *password;
    password = (char *)Pass_Word.toStdString().c_str();
    rc = ssh_userauth_password(BP_session, NULL, password);
    if (rc != SSH_AUTH_SUCCESS){
        ui->ConnectionState->setText("Disconnect");
    }

    ///// Make Channel /////
    BPchannel = ssh_channel_new(BP_session);
    rc = ssh_channel_open_session(BPchannel);

    ///// Request_exec /////
    QString Empt = " ";
    QString Np = ui->Np_vel_2->text();
    QString Nc = ui->Nc_vel_2->text();
    QString rw = ui->rw_vel_2->text();
    QString Command = "cd release; sudo ./TCP_test_GUI 0 1" + Empt + HOST + Empt + PORT + Empt + Np + Empt + Nc + Empt + rw;

    rc = ssh_channel_request_exec(BPchannel, Command.toStdString().c_str());
    if (rc != SSH_OK){
        ui->ConnectionState->setText("Disconnect");
    }

    ssh_channel_send_eof(BPchannel);
    ssh_channel_close(BPchannel);
    ssh_channel_free(BPchannel);

    ssh_disconnect(BP_session);
    ssh_free(BP_session);

    ui->State_2->setText("Running...");
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

double string_to_double( const string& s )
{
  istringstream i(s);
  double x;
  if (!(i >> x))
    return 0;
  return x;
}

void plotNetworkData::on_plotButton_clicked()
{
    QVector<double> x_Mag, y_Mag, x_Phase, y_Phase;

    int at_cnt=0, tt_cnt=0, ap_cnt=0, tp_cnt=0, tv_cnt=0, av_cnt=0;

    double Fs = 4000.0;     // Sampling frequency 4[kHz]
    double Fn = Fs/2;       // Nyquist  frequency 2[kHz]
    double Et = 10.0;       // End time
    int L = (int) Et*Fs;    // Length of signal
    const int nbin = 65536; // Next power of 2 from length of signal

    double act_tor[nbin]={}, tar_tor[nbin]={};
    double act_pos[nbin]={}, tar_pos[nbin]={};
    double act_vel[nbin]={}, tar_vel[nbin]={};

    double data_act[nbin]={}, data_tar[nbin]={};

    double TF_Mag[nbin]={}, TF_Phase[nbin]={};
    double act_Mag, tar_Mag;
    double act_Phase, tar_Phase;

    int IndexNum = (int)(nbin/2);
    double FrqVec[IndexNum+1]={};
    for (int i = 0; i < IndexNum + 1; ++i ){
        FrqVec[i] = (i*Fn)/(IndexNum);
    }

    fstream fs;
    string str_buf;
    string data;
    vector<string> DATA;

    QString sModulePath = QCoreApplication::applicationDirPath();
    qDebug()  << "sModulePath:"<< sModulePath;

    fs.open("CORE200_LoadTest_Data.csv",ios::in);

    // Contain first information index (L+1)
    for (int i = 0; i < L+1; ++i ) {
        getline(fs,str_buf);
        stringstream sstream(str_buf);
        for (int j = 0; j < 12; ++j ){
                getline(sstream,data,',');
                DATA.push_back(data);
                QString qdata = QString::fromStdString(data);
            }
    }

    for (int k = 0; k < (int)DATA.size(); k++){
        if (k > 11){
            if (k%12 == 1){
                act_tor[at_cnt] = string_to_double(DATA[k]);
                at_cnt = at_cnt + 1;
            }
            if (k%12 == 2){
                tar_tor[tt_cnt] = string_to_double(DATA[k]);
                tt_cnt = tt_cnt + 1;
            }
            if (k%12 == 3){
                act_pos[ap_cnt] = string_to_double(DATA[k]);
                ap_cnt = ap_cnt + 1;
            }
            if (k%12 == 4){
                tar_pos[tp_cnt] = string_to_double(DATA[k]);
                tp_cnt = tp_cnt + 1;
            }
            if (k%12 == 6){
                tar_vel[tv_cnt] = string_to_double(DATA[k]);
                tv_cnt = tv_cnt + 1;
            }
            if (k%12 == 7){
                act_vel[av_cnt] = string_to_double(DATA[k]);
                av_cnt = av_cnt + 1;
            }
        }
    }
    qDebug()  << "act_ini[iq][0]"<< act_pos[0];
    fs.close();

    fftw_complex *act_ini, *act_fin;
    act_ini = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nbin);
    act_fin = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nbin);
    fftw_complex *tar_ini, *tar_fin;
    tar_ini = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nbin);
    tar_fin = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * nbin);
    fftw_plan act_fft;
    act_fft = fftw_plan_dft_1d(nbin, act_ini, act_fin, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_plan tar_fft;
    tar_fft = fftw_plan_dft_1d(nbin, tar_ini, tar_fin, FFTW_FORWARD, FFTW_ESTIMATE);

    // Data select & copy
    if (CtrlMode==0){
        memcpy(&data_act, &act_pos, sizeof(act_pos));
        memcpy(&data_tar, &tar_pos, sizeof(tar_pos));
    }
    if (CtrlMode==1){
        memcpy(&data_act, &act_vel, sizeof(act_vel));
        memcpy(&data_tar, &tar_vel, sizeof(tar_vel));
    }
    if (CtrlMode==2){
        memcpy(&data_act, &act_tor, sizeof(act_tor));
        memcpy(&data_tar, &tar_tor, sizeof(tar_tor));
    }

    for (int iq = 0; iq < nbin; iq++) {
        act_ini[iq][0] = data_act[iq];   // Real
        act_ini[iq][1] = 0.0;            // Imag
        tar_ini[iq][0] = data_tar[iq];   // Real
        tar_ini[iq][1] = 0.0;            // Imag
    }

    fftw_execute(act_fft);
    fftw_execute(tar_fft);

    for (int n = 0; n < nbin ; n++){
        act_Mag   = sqrt(pow(act_fin[n][0], 2.0) + pow(act_fin[n][1], 2.0));
        act_Phase = atan2(act_fin[n][1], act_fin[n][0]);

        tar_Mag   = sqrt(pow(tar_fin[n][0], 2.0) + pow(tar_fin[n][1], 2.0));
        tar_Phase = atan2(tar_fin[n][1], tar_fin[n][0]);

        TF_Mag[n]   = 20*log10(act_Mag/tar_Mag);
        TF_Phase[n] = (act_Phase - tar_Phase)*180.0/PI;

        if (TF_Phase[n] > 90){
            TF_Phase[n] = -720 + TF_Phase[n];
        }
    }

    for (int i=0; i<IndexNum+1; i++){
        x_Mag.push_back(FrqVec[i]);
        y_Mag.push_back(TF_Mag[i]);
        x_Phase.push_back(FrqVec[i]);
        y_Phase.push_back(TF_Phase[i]);
    }

    // 동적으로 할당된 메모리 해제
    fftw_destroy_plan(act_fft);
    fftw_destroy_plan(tar_fft);
    fftw_free(act_ini);
    fftw_free(act_fin);
    fftw_free(tar_ini);
    fftw_free(tar_fin);

    ui->MagPlot->graph(0)->setData(x_Mag, y_Mag);
    ui->MagPlot->rescaleAxes();
    ui->MagPlot->replot();
    ui->MagPlot->update();

    ui->PhasePlot->graph(0)->setData(x_Phase, y_Phase);
    ui->PhasePlot->rescaleAxes();
    ui->PhasePlot->replot();
    ui->PhasePlot->update();
}

void plotNetworkData::on_tabWidget_currentChanged(int index)
{
    Mode = index;
    if (Mode == 0){
        CtrlMode = ui->plotTypeComboBox->currentIndex();
        Controller = ui->ControllerComboBox->currentIndex();
    }
    else if(Mode == 1){
        CtrlMode = ui->plotTypeComboBox_2->currentIndex();
        Controller = ui->ControllerComboBox_2->currentIndex();
    }
}

void plotNetworkData::on_copyButton_clicked()
{
    // Copy .csv from step pc

    QString Step_IP = ui->StepIP->text();
    QString User_ID = ui->UserID->text();
    QString Pass_Word = ui->Password->text();
    int size, mode;
    char *filename, *buffer;

    ///// Open session and set options /////
    BP_session = ssh_new();
    if (BP_session == NULL)
      exit(-1);
    ssh_options_set(BP_session, SSH_OPTIONS_HOST, Step_IP.toStdString().c_str());
    ssh_options_set(BP_session, SSH_OPTIONS_USER, User_ID.toStdString().c_str());

    ///// Connect to server /////
    rc = ssh_connect(BP_session);
    if (rc != SSH_OK){
        ui->ConnectionState->setText("Disconnect");
    }

    ///// Password /////
    char *password;
    password = (char *)Pass_Word.toStdString().c_str();
    rc = ssh_userauth_password(BP_session, NULL, password);
    if (rc != SSH_AUTH_SUCCESS){
        ui->ConnectionState->setText("Disconnect");
    }

    scp = ssh_scp_new
      (BP_session, SSH_SCP_READ, "release/CORE200_LoadTest_Data.csv");
    if (scp == NULL){
      fprintf(stderr, "Error allocating scp session: %s\n",
              ssh_get_error(BP_session));
    }

    rc = ssh_scp_init(scp);
    if (rc != SSH_OK){
      fprintf(stderr, "Error initializing scp session: %s\n",
              ssh_get_error(BP_session));
      ssh_scp_free(scp);
    }

    rc = ssh_scp_pull_request(scp);
    if (rc != SSH_SCP_REQUEST_NEWFILE){
      fprintf(stderr, "Error receiving information about file: %s\n", ssh_get_error(BP_session));
    }

    size = ssh_scp_request_get_size(scp);
    filename = strdup(ssh_scp_request_get_filename(scp));
    mode = ssh_scp_request_get_permissions(scp);
    printf("Receiving file %s, size %d, permissions 0%o\n", filename, size, mode);
    free(filename);

    buffer = (char *)malloc(size);
    if (buffer == NULL){
      fprintf(stderr, "Memory allocation error\n");
    }

    ssh_scp_accept_request(scp);

    int r = 0;

    while (r < size){
        int st = ssh_scp_read(scp, buffer+r, size-r);
        r += st;
    }

    FILE*fp = fopen("CORE200_LoadTest_Data.csv","w");

    fwrite(buffer, size, 1, fp);
    free(buffer);

    rc = ssh_scp_pull_request(scp);
    if (rc != SSH_SCP_REQUEST_EOF){
      fprintf(stderr, "Unexpected request: %s\n",
              ssh_get_error(BP_session));
    }

    fclose(fp);

    ssh_scp_close(scp);
    ssh_scp_free(scp);

    ssh_disconnect(BP_session);
    ssh_free(BP_session);

    ui->State_2->setText("Copy complete!");
}
