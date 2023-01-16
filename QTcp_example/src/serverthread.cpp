#include "serverthread.h"

ServerThread::ServerThread(QObject *parent)
{

}

void ServerThread::run()
{

    MyServer server;
    connect(&server,SIGNAL(newDataRecieved(QVector<double> ,QVector<double> )),this,SIGNAL(newDataRecieved(QVector<double> ,QVector<double> )));

    server.startServer();
    exec();
}
