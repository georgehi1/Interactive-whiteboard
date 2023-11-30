#include "communication.h"

#include <stdio.h>
#include <bitset>
#include <QDebug>
#include <pigpio.h>
#include <chrono>


#define uSecondsPerSample 400       //Introduce a sampling limit in the amount of uS to allow final bit of message to be read properly


#define bitsetLength 12              //The amount of bits each binary message contains + 1 control bit. The MSB is a control bit
                                     //FHD:     1920x1080 bitsetLength >= 12
                                     //4K UHD:  840x2160 bitsetLength >= 13
                                     //Increase bitSetLength for higher resolutions
//Output pins
#define xOutPin 20
#define yOutPin 16
#define ROutPin 1
#define SOutPin 8

//Input pins
#define xInPin  26
#define yInPin  19
#define RInPin  0
#define SInPin  11

//Control variables (S is controlled by sender, R is controlled by reciever)
int S = 0;
int R = 0;




CircularQueue<std::string> xOutQueue(1000);               //Each x and y coordinate are coverted to binary strings and held in the queue
CircularQueue<std::string> yOutQueue(1000);               //Each string can then be dequeued and traversed to send each bit individually
                                                          //The size of each queue should be increased if the data rate is too slow and queues are getting full
CircularQueue<std::string> xInQueue(1000);                //however keep it below 4000 or you will get alloc errors
CircularQueue<std::string> yInQueue(1000);





void initialiseCommunication(){
    if (gpioInitialise() < 0){
        exit(0);
    }
    else{
        gpioSetMode(xOutPin, PI_OUTPUT);     //Data Out (x for both devices)
        gpioSetMode(yOutPin, PI_OUTPUT);     //Data Out (y for both devices)
        gpioSetMode(xInPin, PI_INPUT);       //Data In  (x for both devices)
        gpioSetMode(yInPin, PI_INPUT);       //Data In  (y for both devices)

        gpioSetMode(ROutPin, PI_OUTPUT);     //Control Out (C control)
        gpioSetMode(SOutPin, PI_OUTPUT);     //Control Out (S control)
        gpioSetMode(RInPin, PI_INPUT);       //Control In  (C control)
        gpioSetMode(SInPin, PI_INPUT);       //Control In (S control)


        gpioWrite(xOutPin, 0);
        gpioWrite(yOutPin, 0);
        gpioWrite(ROutPin, 0);
        gpioWrite(SOutPin, 0);
    };
}




std::string IntegerToBinary(int data){
    std::bitset<bitsetLength> binary = data;
    return binary.to_string();
}


void loadIntoSendXQueue(int x){
    std::string xBinary = IntegerToBinary(x);
    if (xOutQueue.enqueue(xBinary) == -1){
        qDebug() << "Error: X Out Queue is full";
    }
}
void loadIntoSendYQueue(int y){
    std::string yBinary = IntegerToBinary(y);
    if (yOutQueue.enqueue(yBinary) == -1){
        qDebug() << "Error: Y Out Queue is full";
    }
}


//Communication protocol: Send can onyl send a bit if the S control bit == the R control bit. Recieve can only recieev a bit if
//the R control bit != S control bit. Only send has control over S and only recieve has control over R. When either send or recieve
//is acivated it will invert its respective bit locking it out of getting activated again. The other device which wasnt activated will
//then be available to be activated and when ready will invert its bit allowing the initial device to be available to be activated again.
bool sendX(){
        std::string xBinary = xOutQueue.dequeue();
        if (xBinary != "empty"){
            xBinary[0] = '1';
            unsigned long int i = 0;
            while(i < bitsetLength){
                 if (S == gpioRead(RInPin)){
                    std::string bit(1, xBinary[i]);
                    gpioWrite(xOutPin, std::stoi(bit));
                    qDebug() << "SendX bit"<< i << ":   " <<std::stoi(bit);
                    S = !S;
                    gpioWrite(SOutPin, S);
                    i++;
                 }
            }
            gpioDelay(uSecondsPerSample);
            gpioWrite(xOutPin, 0);
            return true;
        } else {
            return false;
        }
}
bool sendY(){
        std::string yBinary = yOutQueue.dequeue();
        if (yBinary != "empty"){
            yBinary[0] = '1';
            unsigned long int i = 0;
            while (i < bitsetLength){
                if (S == gpioRead(RInPin)){
                    std::string bit(1, yBinary[i]);
                    gpioWrite(yOutPin, std::stoi(bit));
                    qDebug() << "SendY bit"<< i << ":   " <<std::stoi(bit);
                    S = !S;
                    gpioWrite(SOutPin, S);
                    i++;
                }
            }
            gpioDelay(uSecondsPerSample);
            gpioWrite(yOutPin, 0);
            return true;
        } else {
            return false;
        }
}
bool recieveX(){
        int level = gpioRead(xInPin);
        if (level == 1){
            std::string xBinary;
            int i = 0;
            while (i < bitsetLength){
                if (gpioRead(SInPin) != R){
                    xBinary += std::to_string(gpioRead(xInPin));
                    qDebug() << "RecieveX bit" << i << ":   " <<xBinary[i];
                    R = !R;
                    gpioWrite(ROutPin, R);
                    i++;
                }
            }
            if (xInQueue.enqueue(xBinary) == -1){
                qDebug() << "Error: X In Queue is full";
            }
            return true;
        } else {
            return false;
        }
}
bool recieveY(){
        int level = gpioRead(yInPin);
        if (level == 1){
            std::string yBinary;
            int i = 0;
            while (i < bitsetLength){
                if (gpioRead(SInPin) != R){
                    yBinary += std::to_string(gpioRead(yInPin));
                    qDebug() << "RecieveY bit" << i << ":   " <<yBinary[i];
                    R = !R;
                    gpioWrite(ROutPin, R);
                    i++;
                }
            }
            if (yInQueue.enqueue(yBinary) == -1){
                qDebug() << "Error: Y In Queue is full";
            }
            return true;
        } else {
            return false;
        }
}

//Loops that the send and recieve threads loop through to keep sending and recieveing information permanently. Ensures x is sent before y
void send()
{
    bool xDone = false;
    bool yDone = false;
       while(true){
        while(xDone == false){
            xDone = sendX();
        }
        while(yDone == false){
            yDone = sendY();
        }
        xDone = false;
        yDone = false;
    }
}
void recieve()
{
    bool xDone = false;
    bool yDone = false;
    while(true){
        while(xDone == false){
            xDone = recieveX();
        }
        while(yDone == false){
          yDone = recieveY();
        }
        xDone = false;
        yDone = false;
    }
}


//Debug the values of each queue in the terminal
void testView(){
    qDebug() << "x out queue \n";
    xOutQueue.testView();
    qDebug() << "y out queue \n";
    yOutQueue.testView();

    qDebug() << "x in queue \n";
    xInQueue.testView();
    qDebug() << "y in queue \n";
    yInQueue.testView();
}
