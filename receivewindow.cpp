#include "receivewindow.h"
#include "mainwindow.h"
#include "communication.h"
#include "queue.h"
#include "receivedrawingarea.h"
#include "drawingarea.h"
#include <pigpio.h>
#include <thread>


ReceiveWindow::ReceiveWindow(QWidget *parent) : QMainWindow(parent)
{

    if (gpioInitialise() < 0) {
        qDebug() << "Error: Failed to initialize GPIO library";
    }

    gpioSetMode(xInPin, PI_INPUT);
    gpioSetMode(yInPin, PI_INPUT);
    gpioSetMode(RInPin, PI_INPUT);
    gpioSetMode(SInPin, PI_INPUT);


    gpioSetMode(ROutPin, PI_OUTPUT);
    gpioSetMode(SOutPin, PI_OUTPUT);


    gpioWrite(ROutPin, R);
    gpioWrite(SOutPin, S);


    // Create the drawing area
    ReceiveDrawingArea *drawingArea = new ReceiveDrawingArea(this);
    setCentralWidget(drawingArea);

    // Start the thread to receive drawing data
    std::thread receiveThread(&ReceiveWindow::receiveDrawing, this);
    receiveThread.detach();
}

void ReceiveWindow::receiveDrawing()
{
    while (true) {
        //ensure the queues arent empty
        if ((xInQueue.isEmpty() == 1) || (yInQueue.isEmpty() == 1)){
            continue;
        }

        std::string xBinary = xInQueue.dequeue();
        std::string yBinary = yInQueue.dequeue();

        // Remove the control bit
        xBinary = xBinary.substr(1);
        yBinary = yBinary.substr(1);


        int x = binaryToInteger(xBinary);
        int y = binaryToInteger(yBinary);

        //Check if the next coordinate is far away from the previous coordinate so they arent connected by a line
        if ((x - prevX > 60) || (x - prevX < -60) || (y - prevY > 60) || (y - prevY < -60)){
            prevX = x;
            prevY = y;
            continue;
        }

        // Draw a line from the previous coordinate to the new coordinate
        static_cast<ReceiveDrawingArea*>(centralWidget())->drawLine(prevX, prevY, x, y);

        // Update the previous coordinate values
        prevX = x;
        prevY = y;


        update();

    }
}

void ReceiveWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);

    // Draw the current state of the window
    QPainter painter(this);
    painter.drawText(20, 20, "Receive Window");
    painter.drawText(20, 40, QString("R = %1, S = %2").arg(R).arg(S));
}

int binaryToInteger(const std::string& bits)
{
    int value = 0;
    int base = 1;

    for (int i = bits.size() - 1; i >= 0; i--) {
        if (bits[i] == '1') {
            value += base;
        }
        base *= 2;
    }

    return value;
}

