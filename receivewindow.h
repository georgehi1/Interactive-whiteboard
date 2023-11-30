#ifndef RECEIVEWINDOW_H
#define RECEIVEWINDOW_H

#include <QMainWindow>
#include <QPainter>

//Input pins
#define xInPin  26
#define yInPin  19
#define RInPin  0
#define SInPin  11

//Output pins
#define ROutPin 1
#define SOutPin 8

class ReceiveWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReceiveWindow(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;

private:
    QPainter painter;
    int prevX = 0;
    int prevY = 0;

    void receiveDrawing();

    int R = 0;
    int S = 0;
};

int binaryToInteger(const std::string& bits);

#endif // RECEIVEWINDOW_H
