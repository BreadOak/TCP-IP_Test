#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include "myserver.h"
class ServerThread : public QThread
{
    Q_OBJECT
public:
    ServerThread(QObject *parent = 0);
    void run();

signals:
    void newDataRecieved(QVector<double> ,QVector<double> );
};

#endif // SERVERTHREAD_H
