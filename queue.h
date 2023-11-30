#ifndef QUEUE_H
#define QUEUE_H

#include <QDebug>

template <class T>
class CircularQueue
{
public:
    CircularQueue(int inputSize){
        array = new T[inputSize];
        head = 0;
        tail = 0;
        privateSize = inputSize;
        capacity = 0;
    };


    ~CircularQueue(){
        delete[] array;
    };


    int enqueue(T data){
        if (capacity == privateSize){
            return -2;
        };
        array[tail] = data;
        if (tail == privateSize - 1){
            tail = 0;
        }
        else {
            tail += 1;
        }
        capacity += 1;
        return -9;
    };


    T dequeue(){
        if (capacity == 0){
            return "empty";
        };
        T data = array[head];
        if (head == privateSize - 1){
            head = 0;
        }
        else {
            head += 1;
        }
        capacity -= 1;
        return data;
    };


    T peak(){
        return array[head];
    };

    int isEmpty() {
        if(capacity == 0){
            return 1;
        }
    }

    void testView(){
      int temp;
      if(head < tail){
        temp = head;
        for(temp; temp != tail; temp++){
            QString outString = QString::fromStdString(array[temp]);
            qDebug() << temp << "index";
            qDebug() << outString;
        }
      }else if(head > tail){
          temp = tail;
          for(temp; temp != head; temp++){
              QString outString = QString::fromStdString(array[temp]);
              qDebug() << temp << "index";
              qDebug() << outString;
          }
      }

    };

private:
    T* array;
    int head, tail, capacity, privateSize;
};

extern CircularQueue<std::string> xOutQueue;
extern CircularQueue<std::string> yOutQueue;

extern CircularQueue<std::string> xInQueue;
extern CircularQueue<std::string> yInQueue;

#endif // QUEUE_H
