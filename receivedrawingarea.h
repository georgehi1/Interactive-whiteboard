#ifndef RECEIVEDRAWINGAREA_H
#define RECEIVEDRAWINGAREA_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPointF>
#include <QVector>

class ReceiveDrawingArea : public QWidget
{
    Q_OBJECT
public:
    explicit ReceiveDrawingArea(QWidget *parent = nullptr);

    void drawLine(int x1, int y1, int x2, int y2);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QVector<QPointF>> receivedLines;
};

#endif // RECEIVEDRAWINGAREA_H
