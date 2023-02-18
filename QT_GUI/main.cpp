#include "plotnetworkdata.h"

#include <QApplication>
#include <QVector>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QVector<double>>();
    plotNetworkData w;
    w.show();

    return a.exec();
}
