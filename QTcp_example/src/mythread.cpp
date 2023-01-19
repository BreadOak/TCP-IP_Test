#include "mythread.h"
#include <QTextStream>
#include <QVector>

int PlotType;   // Plot type Global variable
int CtrlMode;   // Ctrl mode Global variable
int Controller; // Controller Global variable
double TargetValue;

QTextStream out(stdout);

MyThread::MyThread(qintptr ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
}

void MyThread::run()
{
    // thread starts here
    qDebug() << " Thread started";

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
    qDebug() << socketDescriptor << " Client connected";

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
//    qDebug() << socketDescriptor << " Data in: " << Data;
    parseMsg(Data);

    ///// Write /////
    // Data send vector
    double sVec[10];

    sVec[0] = double(PlotType);   // 0: Position, 1: Velocity, 2: Torque
    sVec[1] = double(CtrlMode);   // 0: Position, 1: Velocity, 2: Torque
    sVec[2] = double(Controller); // 0: PID/PD/FW, 1: None/PD+DOB/None, 2: None/MPC/None, 3: None/MPC+DOB/None
    sVec[3] = TargetValue;
    sVec[4] = 5.5;
    sVec[5] = 6.6;
    sVec[6] = 7.7;
    sVec[7] = 8.8;
    sVec[8] = 9.9;
    sVec[9] = 10.10;

    qDebug() << socketDescriptor << "data:" <<  sVec[3];

    // Double Array to QByteArray
    QByteArray sData;
    sData = QByteArray::fromRawData((char*)sVec, sizeof(sVec));

    // Write Data to client
    socket->write(sData);

    // will write on server side window
//    qDebug() << socketDescriptor << " Data in: " << sData;
}

void MyThread::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";

    socket->deleteLater();
    exit(0);
}

void MyThread::parseMsg(QByteArray Data)
{
    if (count == 0)
    {
        xCorr.clear();
        yCorr.clear();
    }

    // Data recv structure
    struct rData{
        double time[50];
        double data[50];
    }rData;

    memcpy((char *)&rData, Data.data(), sizeof(rData));

    for (int i=0; i<50; ++i)
    {
        xCorr.push_back(rData.time[i]);
        yCorr.push_back(rData.data[i]);

        if (count >= 400)
        {
            xCorr.remove(0);
            yCorr.remove(0);
        }
    }

    // emit signal
    emit newDataRecieved(xCorr,yCorr);

    ++count;
}
