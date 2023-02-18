#include "mythread.h"
#include <QTextStream>
#include <QVector>

int OnOffSignal;
int RunSignal;
int CtrlMode;   // Ctrl mode Global variable
int Controller; // Controller Global variable
int PlotType;   // Plot type Global variable
int DOB_option;
double TargetValue;
double Kp_PosCtrl;
double Ki_PosCtrl;
double Kd_PosCtrl;
double Kp_VelCtrl;
double Ki_VelCtrl;

extern double Endtime;

QTextStream out(stdout);

MyThread::MyThread(qintptr ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
}

void MyThread::run()
{
    // thread starts here
    qDebug() << "Thread started";

    socket = new QTcpSocket();

    // set the ID
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        // something's wrong, we just emit a signal
        emit error(socket->error());
        return;
    }

    // connect socket and signal
    // note - Qt::DirectConnection is used because it's multithreaded
    //        This makes the slot to be invoked immediately, when the signal is emitted.

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    // We'll have multiple clients, we want to know which is which
    qDebug() << socketDescriptor << "Client connected";

    // make this thread a loop,
    // thread will stay alive so that signal/slot to function properly
    // not dropped out in the middle when thread dies

    exec();
}

void MyThread::readyRead()
{
    ///// Read /////
    // Read Data from client
    QByteArray Data = socket->readAll();

    // will write on server side window
    parseMsg(Data);

    ///// Write /////
    // Data send vector
    double sVec[15];

    sVec[0]  = double(OnOffSignal); // 0: Off,       1: On
    sVec[1]  = double(RunSignal);   // 0: Stop,      1: Run
    sVec[2]  = double(CtrlMode);    // 0: Position,  1: Velocity,      2: Torque
    sVec[3]  = double(Controller);  // 0: PID/PD/FW, 1: None/MPC/None, 2: None/H-inf/None
    sVec[4]  = double(PlotType);    // 0: Position,  1: Velocity,      2: Torque
    sVec[5]  = double(DOB_option);  // 0: Off,       1: On
    sVec[6]  = TargetValue;
    sVec[7]  = Kp_PosCtrl;
    sVec[8]  = Ki_PosCtrl;
    sVec[9]  = Kd_PosCtrl;
    sVec[10] = Kp_VelCtrl;
    sVec[11] = Ki_VelCtrl;

//    qDebug() << socketDescriptor << "PlotType:"    <<  sVec[0];
//    qDebug() << socketDescriptor << "CtrlMode:"    <<  sVec[1];
//    qDebug() << socketDescriptor << "Controller:"  <<  sVec[2];
//    qDebug() << socketDescriptor << "TargetValue:" <<  sVec[3];
//    qDebug() << socketDescriptor << "RunSignal:"   <<  sVec[4];
//    qDebug() << socketDescriptor << "OnOffSignal:" <<  sVec[5];
//    qDebug() << socketDescriptor << "Kp_PosCtrl:"  <<  sVec[6];
//    qDebug() << socketDescriptor << "Ki_PosCtrl:"  <<  sVec[7];
//    qDebug() << socketDescriptor << "Kd_PosCtrl:"  <<  sVec[8];
//    qDebug() << socketDescriptor << "Kp_VelCtrl:"  <<  sVec[9];
//    qDebug() << socketDescriptor << "Ki_VelCtrl:"  <<  sVec[10];

    // Double Array to QByteArray
    QByteArray sData;
    sData = QByteArray::fromRawData((char*)sVec, sizeof(sVec));

    // Write Data to client
    socket->write(sData);
}

void MyThread::disconnected()
{
    qDebug() << socketDescriptor << "Disconnected";
    socket->deleteLater();
    exit(0);
}

void MyThread::parseMsg(QByteArray Data)
{
    if (RunSignal){
        if (count == 0){
            xCorr.clear();
            yCorr.clear();
        }

        // Data recv structure
        struct rData{
            double time[50];
            double data[50];
        }rData;

        memcpy((char *)&rData, Data.data(), sizeof(rData));

        Endtime = rData.time[49];

        for (int i=0; i<50; ++i)
        {
            xCorr.push_back(rData.time[i]);
            yCorr.push_back(rData.data[i]);

            if (count >= 400){
                xCorr.remove(0);
                yCorr.remove(0);
            }
        }

        // emit signal
        emit newDataRecieved(xCorr,yCorr);
        ++count;
    }
}
