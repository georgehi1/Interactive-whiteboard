#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <string>
#include <pigpio.h>
#include "queue.h"

void initialiseCommunication();
int BinaryToInteger();
std::string IntegerToBinary();
void loadIntoSendXQueue(int x);
void loadIntoSendYQueue(int y);
bool sendX();
bool sendY();
bool recieveX();
bool recieveY();
void send();
void recieve();
void testView();


#endif // COMMUNICATION_H
