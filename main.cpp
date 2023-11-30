#include "mainwindow.h"
#include "communication.h"
#include "receivewindow.h"

#include <QApplication>
#include <iostream>
#include <pigpio.h>
#include <thread>
#include <pthread.h>
#include <mutex>
#include <chrono>

int main(int argc, char *argv[])
{
    initialiseCommunication();

    std::thread SendThread(send);
    SendThread.detach();
    std::thread RecieveThread(recieve);
    RecieveThread.detach();



    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    ReceiveWindow receiveWindow;
    receiveWindow.show();



    return a.exec();

    gpioTerminate();


}
