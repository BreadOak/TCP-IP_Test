#ifndef MYTHREAD_H
#define MYTHREAD_H
#include "plotnetworkdata.h"
#include <QtNetwork>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(qintptr ID, QObject *parent = 0);

    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void newDataRecieved(QVector<double> ,QVector<double> );

public slots:
    void readyRead();
    void disconnected();

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    QVector<double> xCorr,yCorr;
    int count = 0;
    void parseMsg(QByteArray Data);
};

#endif // MYTHREAD_H
