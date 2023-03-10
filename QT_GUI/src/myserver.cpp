#include "myserver.h"

#include "myserver.h"
#include "mythread.h"

MyServer::MyServer(QObject *parent) :
    QTcpServer(parent)
{

}

void MyServer::startServer()
{
    int port = 8000;

    if(!this->listen(QHostAddress::Any,port)){
        qDebug() << "Could not start server";
    }
    else{
        qDebug() << "Listening to port " << port << "...";
    }
}

void MyServer::incomingConnection(qintptr socketDescriptor)
{
    // We have a new connection
    qDebug() << socketDescriptor << " Connecting...";

    MyThread *thread = new MyThread(socketDescriptor, this);

    // connect signal/slot
    // once a thread is not needed, it will be beleted later
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread,SIGNAL(newDataRecieved(QVector<double>,QVector<double>)),this,SIGNAL(newDataRecieved(QVector<double>,QVector<double>)));
    thread->start();
}
