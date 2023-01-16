#include "mythread.h"
#include <QTextStream>
#include <QVector>

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
    // Read Data from client
    QByteArray Data = socket->readAll();

    // will write on server side window
    qDebug() << socketDescriptor << " Data in: " << Data;
    parseMsg(Data);

    // Data vector
    double vec[2];
    vec[0] = 5.5;
    vec[1] = 6;

    // Double Array to QByteArray
    QByteArray sData;
    sData = QByteArray::fromRawData(reinterpret_cast<const char*>(vec), sizeof(vec)*sizeof(double));

    // Write Data to client
    socket->write(sData);

    // will write on server side window
    qDebug() << socketDescriptor << " Data in: " << sData;

}

void MyThread::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";

    socket->deleteLater();
    exit(0);
}

void MyThread::parseMsg(QByteArray Data){

    double rData[2];

    xCorr.clear();
    yCorr.clear();

    memcpy(rData, Data.data(), sizeof(rData));
    qDebug() << "data1" << rData[0] ;
    qDebug() << "data2" << rData[1] ;

    xCorr.push_back(rData[0]);
    yCorr.push_back(rData[1]);

    // emit signal
    emit newDataRecieved(xCorr,yCorr);
}
